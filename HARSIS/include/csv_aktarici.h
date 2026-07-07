#pragma once

/**
 * csv_aktarici.h -- Toplu CSV Veri Aktarıcı
 *
 * QThread tabanlı CSV dosyası okuma/yazma servisi.
 * Büyük veri setlerini arka planda işler.
 */

#include "varliklar.h"
#include "depo.h"

#include <QThread>
#include <QMutex>
#include <atomic>

class CsvAktarici : public QThread {
    Q_OBJECT

public:
    explicit CsvAktarici(Depo<std::string, Hasta>* hastalar, QObject* parent = nullptr);
    ~CsvAktarici();

    void csvdenAktar(const QString& dosyaYolu);
    void csveAktar(const QString& dosyaYolu);
    void iptalEt();

signals:
    void ilerlemeDegisti(int yuzde, const QString& mesaj);
    void aktarimTamamlandi(int basariliSayisi, int hataliSayisi);
    void hataOlustu(const QString& mesaj);

protected:
    void run() override;

private:
    Depo<std::string, Hasta>* m_hastalar;
    QMutex m_mutex;
    std::atomic<bool> m_iptalEdildi;
    
    QString m_dosyaYolu;
    bool m_aktarimYonu; // true: içe aktar, false: dışa aktar
    
    void csvdenOku();
    void csvyeYaz();
};
