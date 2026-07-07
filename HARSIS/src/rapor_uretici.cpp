#include "../include/rapor_uretici.h"

#include <QDateTime>
#include <QMutexLocker>

RaporUretici::RaporUretici(Depo<int, Randevu>* randevular,
                           Depo<std::string, Doktor>* doktorlar,
                           QObject* parent)
    : QThread(parent), m_randevular(randevular), m_doktorlar(doktorlar), m_iptalEdildi(false) {
}

RaporUretici::~RaporUretici() {
    iptalEt();
    wait();
}

void RaporUretici::raporUret(const QString& baslangicTarihi, const QString& bitisTarihi) {
    QMutexLocker locker(&m_mutex);
    m_baslangicTarihi = baslangicTarihi;
    m_bitisTarihi = bitisTarihi;
    m_iptalEdildi = false;
    
    if (!isRunning()) {
        start();
    }
}

void RaporUretici::iptalEt() {
    m_iptalEdildi = true;
}

void RaporUretici::run() {
    try {
        emit ilerlemeDegisti(0);
        
        if (m_iptalEdildi) return;
        
        RaporSonucu sonuc = raporOlustur();
        
        if (m_iptalEdildi) return;
        
        emit ilerlemeDegisti(100);
        emit raporHazir(sonuc);
        
    } catch (const std::exception& e) {
        emit hataOlustu(QString("Rapor oluşturma hatası: %1").arg(e.what()));
    }
}

RaporSonucu RaporUretici::raporOlustur() {
    QMutexLocker locker(&m_mutex);
    
    RaporSonucu sonuc;
    sonuc.toplamRandevu = 0;
    sonuc.onaylananRandevu = 0;
    sonuc.bekleyenRandevu = 0;
    sonuc.iptalEdilenRandevu = 0;
    sonuc.tamamlananRandevu = 0;
    sonuc.olusturmaTarihi = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    
    if (!m_randevular || !m_doktorlar) {
        throw std::runtime_error("Veri depoları bulunamadı!");
    }
    
    auto randevular = m_randevular->tumunu_al();
    int toplam = static_cast<int>(randevular.size());
    int islenmisSayisi = 0;
    
    emit ilerlemeDegisti(10);
    
    for (const auto& [id, randevu] : randevular) {
        if (m_iptalEdildi) return sonuc;
        
        // Tarih aralığı kontrolü
        if (!tarihAraligindaMi(randevu.tarih)) {
            continue;
        }
        
        sonuc.toplamRandevu++;
        
        // Durum bazlı sayım
        switch (randevu.durum) {
            case RandevuDurum::Onaylandi:
                sonuc.onaylananRandevu++;
                break;
            case RandevuDurum::Beklemede:
                sonuc.bekleyenRandevu++;
                break;
            case RandevuDurum::Iptal:
                sonuc.iptalEdilenRandevu++;
                break;
            case RandevuDurum::Tamamlandi:
                sonuc.tamamlananRandevu++;
                break;
        }
        
        // Branş bazlı dağılım
        auto doktor = m_doktorlar->bul(randevu.sicil_no);
        if (doktor.has_value()) {
            sonuc.bransaBaziDagilim[doktor->brans]++;
        }
        
        // İlerleme güncelle
        islenmisSayisi++;
        int yuzde = 10 + (islenmisSayisi * 80 / toplam);
        emit ilerlemeDegisti(yuzde);
        
        // CPU'ya biraz nefes aldır
        if (islenmisSayisi % 10 == 0) {
            QThread::msleep(10);
        }
    }
    
    emit ilerlemeDegisti(90);
    
    return sonuc;
}

bool RaporUretici::tarihAraligindaMi(const std::string& tarih) const {
    if (m_baslangicTarihi.isEmpty() || m_bitisTarihi.isEmpty()) {
        return true; // Tarih aralığı belirtilmemişse tümünü al
    }
    
    QString tarihStr = QString::fromStdString(tarih);
    QDate randevuTarihi = QDate::fromString(tarihStr, "yyyy-MM-dd");
    
    // Kullanıcıdan DD-MM-YYYY formatında geldiği için çeviriyoruz
    QDate baslangic = QDate::fromString(m_baslangicTarihi, "dd-MM-yyyy");
    QDate bitis = QDate::fromString(m_bitisTarihi, "dd-MM-yyyy");
    
    if (!randevuTarihi.isValid() || !baslangic.isValid() || !bitis.isValid()) {
        return false;
    }
    
    return randevuTarihi >= baslangic && randevuTarihi <= bitis;
}
