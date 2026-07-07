#pragma once

/**
 * rapor_uretici.h -- Arka Plan Rapor Üretici
 *
 * QThread tabanlı rapor üretim servisi.
 * Günlük/haftalık randevu istatistiklerini oluşturur.
 */

#include "varliklar.h"
#include "depo.h"

#include <QThread>
#include <QMutex>
#include <atomic>
#include <map>

struct RaporSonucu {
    int toplamRandevu;
    int onaylananRandevu;
    int bekleyenRandevu;
    int iptalEdilenRandevu;
    int tamamlananRandevu;
    std::map<std::string, int> bransaBaziDagilim;
    QString olusturmaTarihi;
};

class RaporUretici : public QThread {
    Q_OBJECT

public:
    explicit RaporUretici(Depo<int, Randevu>* randevular,
                         Depo<std::string, Doktor>* doktorlar,
                         QObject* parent = nullptr);
    ~RaporUretici();

    void raporUret(const QString& baslangicTarihi, const QString& bitisTarihi);
    void iptalEt();

signals:
    void ilerlemeDegisti(int yuzde);
    void raporHazir(const RaporSonucu& sonuc);
    void hataOlustu(const QString& mesaj);

protected:
    void run() override;

private:
    Depo<int, Randevu>* m_randevular;
    Depo<std::string, Doktor>* m_doktorlar;
    QMutex m_mutex;
    std::atomic<bool> m_iptalEdildi;
    
    QString m_baslangicTarihi;
    QString m_bitisTarihi;
    
    RaporSonucu raporOlustur();
    bool tarihAraligindaMi(const std::string& tarih) const;
};
