#pragma once

/**
 * hatirlatma_servisi.h -- Randevu Hatırlatma Servisi
 *
 * QThread tabanlı arka plan servisi.
 * Yaklaşan randevuları tarayarak bildirim oluşturur.
 */

#include "varliklar.h"
#include "depo.h"

#include <QThread>
#include <QMutex>
#include <atomic>

class HatirlatmaServisi : public QThread {
    Q_OBJECT

public:
    explicit HatirlatmaServisi(Depo<int, Randevu>* randevular, QObject* parent = nullptr);
    ~HatirlatmaServisi();

    void durdur();
    void taramaAraliginiAyarla(int saniye);

signals:
    void hatirlatmaOlustu(const QString& mesaj);
    void taramaBasladi(int toplamRandevu);
    void taramaBitti(int bulunanRandevu);
    void hataOlustu(const QString& mesaj);

protected:
    void run() override;

private:
    Depo<int, Randevu>* m_randevular;
    QMutex m_mutex;
    std::atomic<bool> m_calisiyorMu;
    int m_taramaAraligi; // saniye cinsinden
    
    void randevulariTara();
    bool yaklasanRandevuMu(const Randevu& randevu) const;
};
