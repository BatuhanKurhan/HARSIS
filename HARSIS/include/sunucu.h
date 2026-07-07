#pragma once

/**
 * sunucu.h -- HARSIS Sunucu
 *
 * TCP/IP tabanlı sunucu sınıfı.
 * Birden fazla istemciye hizmet verir.
 */

#include "depo.h"
#include "varliklar.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <memory>

class Sunucu : public QTcpServer {
    Q_OBJECT

public:
    explicit Sunucu(QObject* parent = nullptr);
    ~Sunucu();
    
    // Sunucu kontrolü
    bool basla(quint16 port = 8080);
    void durdur();
    bool calisiyor() const { return isListening(); }
    
    // Veri depolarını ayarla
    void setHastalar(Depo<std::string, Hasta>* hastalar);
    void setDoktorlar(Depo<std::string, Doktor>* doktorlar);
    void setRandevular(Depo<int, Randevu>* randevular);

signals:
    void yeniIstemci(const QString& adres);
    void istemciAyrildi(const QString& adres);
    void mesajAlindi(const QString& istemci, const QString& mesaj);
    void hata(const QString& hataMesaji);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void istemciMesajiOku();
    void istemciBaglantisiKoptu();

private:
    void mesajIsle(QTcpSocket* istemci, const QJsonObject& mesaj);
    void yanit_gonder(QTcpSocket* istemci, const QJsonObject& yanit);
    
    // Komut işleyicileri
    QJsonObject hasta_listele();
    QJsonObject hasta_ekle(const QJsonObject& veri);
    QJsonObject hasta_bul(const QString& tc);
    QJsonObject doktor_listele();
    QJsonObject doktor_ekle(const QJsonObject& veri);
    QJsonObject randevu_listele();
    QJsonObject randevu_olustur(const QJsonObject& veri);
    
    QList<QTcpSocket*> m_istemciler;
    
    // Veri depoları (referans)
    Depo<std::string, Hasta>* m_hastalar;
    Depo<std::string, Doktor>* m_doktorlar;
    Depo<int, Randevu>* m_randevular;
    int m_sonRandevuId;
};
