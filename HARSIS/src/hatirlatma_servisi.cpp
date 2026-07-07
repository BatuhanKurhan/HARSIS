#include "../include/hatirlatma_servisi.h"

#include <QDateTime>
#include <QMutexLocker>

HatirlatmaServisi::HatirlatmaServisi(Depo<int, Randevu>* randevular, QObject* parent)
    : QThread(parent), m_randevular(randevular), m_calisiyorMu(false), m_taramaAraligi(60) {
}

HatirlatmaServisi::~HatirlatmaServisi() {
    durdur();
    wait(); // Thread'in bitmesini bekle
}

void HatirlatmaServisi::durdur() {
    m_calisiyorMu = false;
}

void HatirlatmaServisi::taramaAraliginiAyarla(int saniye) {
    QMutexLocker locker(&m_mutex);
    m_taramaAraligi = saniye;
}

void HatirlatmaServisi::run() {
    m_calisiyorMu = true;
    
    while (m_calisiyorMu) {
        try {
            randevulariTara();
        } catch (const std::exception& e) {
            emit hataOlustu(QString("Tarama hatası: %1").arg(e.what()));
        }
        
        // Tarama aralığı kadar bekle (1 saniye aralıklarla kontrol ederek)
        for (int i = 0; i < m_taramaAraligi && m_calisiyorMu; ++i) {
            QThread::sleep(1);
        }
    }
}

void HatirlatmaServisi::randevulariTara() {
    QMutexLocker locker(&m_mutex);
    
    if (!m_randevular) {
        emit hataOlustu("Randevu deposu bulunamadı!");
        return;
    }
    
    auto randevular = m_randevular->tumunu_al();
    emit taramaBasladi(static_cast<int>(randevular.size()));
    
    int bulunanSayisi = 0;
    
    for (const auto& [id, randevu] : randevular) {
        // Sadece onaylanmış randevuları kontrol et
        if (randevu.durum != RandevuDurum::Onaylandi) {
            continue;
        }
        
        if (yaklasanRandevuMu(randevu)) {
            QString mesaj = QString("Hatırlatma: Randevu #%1\n"
                                  "Hasta TC: %2\n"
                                  "Doktor: %3\n"
                                  "Tarih: %4 %5")
                .arg(randevu.randevu_id)
                .arg(QString::fromStdString(randevu.tc_no))
                .arg(QString::fromStdString(randevu.sicil_no))
                .arg(QString::fromStdString(randevu.tarih))
                .arg(QString::fromStdString(randevu.saat));
            
            emit hatirlatmaOlustu(mesaj);
            bulunanSayisi++;
        }
    }
    
    emit taramaBitti(bulunanSayisi);
}

bool HatirlatmaServisi::yaklasanRandevuMu(const Randevu& randevu) const {
    // Randevu tarih ve saatini parse et
    QString tarihSaat = QString("%1 %2")
        .arg(QString::fromStdString(randevu.tarih))
        .arg(QString::fromStdString(randevu.saat));
    
    QDateTime randevuZamani = QDateTime::fromString(tarihSaat, "yyyy-MM-dd HH:mm");
    if (!randevuZamani.isValid()) {
        return false;
    }
    
    QDateTime simdi = QDateTime::currentDateTime();
    
    // Randevu 24 saat içindeyse hatırlat
    qint64 saniyeFarki = simdi.secsTo(randevuZamani);
    
    // 0 ile 24 saat (86400 saniye) arasındaysa
    return (saniyeFarki > 0 && saniyeFarki <= 86400);
}
