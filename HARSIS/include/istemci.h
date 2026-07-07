#pragma once

/**
 * istemci.h -- HARSIS İstemci
 *
 * TCP/IP tabanlı istemci sınıfı.
 * Sunucuya bağlanır ve komutlar gönderir.
 */

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class Istemci : public QObject {
    Q_OBJECT

public:
    explicit Istemci(QObject* parent = nullptr);
    ~Istemci();
    
    // Bağlantı kontrolü
    void baglan(const QString& sunucuAdresi, quint16 port = 8080);
    void baglantiyiKes();
    bool bagli() const;
    
    // Komutlar
    void hastaListele();
    void hastaEkle(const QString& tc, const QString& isim, const QString& soyisim,
                   const QString& dogumTarihi, const QString& telefon);
    void hastaBul(const QString& tc);
    void doktorListele();
    void doktorEkle(const QString& sicil, const QString& isim, const QString& soyisim,
                    const QString& brans, const QStringList& uygunSaatler);
    void randevuListele();
    void randevuOlustur(const QString& tc, const QString& sicil,
                        const QString& tarih, const QString& saat);

signals:
    void baglandi();
    void baglantiKoptu();
    void yanitAlindi(const QJsonObject& yanit);
    void hata(const QString& hataMesaji);

private slots:
    void baglantiKuruldu();
    void baglantiKesildiFonksiyonu();
    void veriOku();
    void hataOlustu(QAbstractSocket::SocketError socketError);

private:
    void komutGonder(const QString& komut, const QJsonObject& veri = QJsonObject());
    
    QTcpSocket* m_soket;
};
