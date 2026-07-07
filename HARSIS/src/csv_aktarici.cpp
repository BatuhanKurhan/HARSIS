#include "../include/csv_aktarici.h"

#include <QFile>
#include <QTextStream>
#include <QMutexLocker>

CsvAktarici::CsvAktarici(Depo<std::string, Hasta>* hastalar, QObject* parent)
    : QThread(parent), m_hastalar(hastalar), m_iptalEdildi(false), m_aktarimYonu(true) {
}

CsvAktarici::~CsvAktarici() {
    iptalEt();
    wait();
}

void CsvAktarici::csvdenAktar(const QString& dosyaYolu) {
    QMutexLocker locker(&m_mutex);
    m_dosyaYolu = dosyaYolu;
    m_aktarimYonu = true;
    m_iptalEdildi = false;
    
    if (!isRunning()) {
        start();
    }
}

void CsvAktarici::csveAktar(const QString& dosyaYolu) {
    QMutexLocker locker(&m_mutex);
    m_dosyaYolu = dosyaYolu;
    m_aktarimYonu = false;
    m_iptalEdildi = false;
    
    if (!isRunning()) {
        start();
    }
}

void CsvAktarici::iptalEt() {
    m_iptalEdildi = true;
}

void CsvAktarici::run() {
    try {
        if (m_aktarimYonu) {
            csvdenOku();
        } else {
            csvyeYaz();
        }
    } catch (const std::exception& e) {
        emit hataOlustu(QString("CSV aktarım hatası: %1").arg(e.what()));
    }
}

void CsvAktarici::csvdenOku() {
    QMutexLocker locker(&m_mutex);
    
    QFile dosya(m_dosyaYolu);
    if (!dosya.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit hataOlustu("CSV dosyası açılamadı!");
        return;
    }
    
    locker.unlock();
    
    QTextStream stream(&dosya);
    
    // Başlık satırını atla
    if (!stream.atEnd()) {
        stream.readLine();
    }
    
    int basariliSayisi = 0;
    int hataliSayisi = 0;
    int satirNo = 1;
    
    emit ilerlemeDegisti(0, "CSV dosyası okunuyor...");
    
    while (!stream.atEnd() && !m_iptalEdildi) {
        QString satir = stream.readLine();
        QStringList alanlar = satir.split(',');
        
        if (alanlar.size() >= 5) {
            Hasta hasta;
            hasta.tc_no = alanlar[0].trimmed().toStdString();
            hasta.isim = alanlar[1].trimmed().toStdString();
            hasta.soyisim = alanlar[2].trimmed().toStdString();
            hasta.dogum_tarihi = alanlar[3].trimmed().toStdString();
            hasta.telefon = alanlar[4].trimmed().toStdString();
            
            // TC no kontrolü
            if (hasta.tc_no.length() == 11) {
                locker.relock();
                if (m_hastalar->ekle(hasta.tc_no, hasta)) {
                    basariliSayisi++;
                } else {
                    hataliSayisi++; // Zaten var
                }
                locker.unlock();
            } else {
                hataliSayisi++;
            }
        } else {
            hataliSayisi++;
        }
        
        satirNo++;
        
        // Her 10 satırda bir ilerleme güncelle
        if (satirNo % 10 == 0) {
            emit ilerlemeDegisti(50, QString("%1 satır işlendi...").arg(satirNo));
            QThread::msleep(5); // CPU'ya nefes aldır
        }
    }
    
    dosya.close();
    
    if (!m_iptalEdildi) {
        emit ilerlemeDegisti(100, "Aktarım tamamlandı!");
        emit aktarimTamamlandi(basariliSayisi, hataliSayisi);
    }
}

void CsvAktarici::csvyeYaz() {
    QMutexLocker locker(&m_mutex);
    
    if (!m_hastalar) {
        emit hataOlustu("Hasta deposu bulunamadı!");
        return;
    }
    
    auto hastalar = m_hastalar->tumunu_al();
    int toplam = static_cast<int>(hastalar.size());
    
    locker.unlock();
    
    QFile dosya(m_dosyaYolu);
    if (!dosya.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit hataOlustu("CSV dosyası oluşturulamadı!");
        return;
    }
    
    QTextStream stream(&dosya);
    
    // Başlık satırı
    stream << "TC_NO,ISIM,SOYISIM,DOGUM_TARIHI,TELEFON\n";
    
    emit ilerlemeDegisti(0, "CSV dosyası yazılıyor...");
    
    int yazilan = 0;
    for (const auto& [tc, hasta] : hastalar) {
        if (m_iptalEdildi) break;
        
        stream << QString::fromStdString(hasta.tc_no) << ","
               << QString::fromStdString(hasta.isim) << ","
               << QString::fromStdString(hasta.soyisim) << ","
               << QString::fromStdString(hasta.dogum_tarihi) << ","
               << QString::fromStdString(hasta.telefon) << "\n";
        
        yazilan++;
        
        if (yazilan % 10 == 0) {
            int yuzde = (yazilan * 100) / toplam;
            emit ilerlemeDegisti(yuzde, QString("%1/%2 kayıt yazıldı...").arg(yazilan).arg(toplam));
            QThread::msleep(5);
        }
    }
    
    dosya.close();
    
    if (!m_iptalEdildi) {
        emit ilerlemeDegisti(100, "Dışa aktarım tamamlandı!");
        emit aktarimTamamlandi(yazilan, 0);
    }
}
