#include "istemci.h"
#include <QHostAddress>
#include <QJsonDocument>

Istemci::Istemci(QObject* parent)
    : QObject(parent)
    , m_soket(new QTcpSocket(this))
{
    connect(m_soket, &QTcpSocket::connected, this, &Istemci::baglantiKuruldu);
    connect(m_soket, &QTcpSocket::disconnected, this, &Istemci::baglantiKesildiFonksiyonu);
    connect(m_soket, &QTcpSocket::readyRead, this, &Istemci::veriOku);
    connect(m_soket, &QTcpSocket::errorOccurred, this, &Istemci::hataOlustu);
}

Istemci::~Istemci() {
    if (m_soket->state() == QAbstractSocket::ConnectedState) {
        m_soket->disconnectFromHost();
    }
}

void Istemci::baglan(const QString& sunucuAdresi, quint16 port) {
    if (m_soket->state() == QAbstractSocket::ConnectedState) {
        emit hata("Zaten bağlı!");
        return;
    }
    
    m_soket->connectToHost(sunucuAdresi, port);
}

void Istemci::baglantiyiKes() {
    if (m_soket->state() == QAbstractSocket::ConnectedState) {
        m_soket->disconnectFromHost();
    }
}

bool Istemci::bagli() const {
    return m_soket->state() == QAbstractSocket::ConnectedState;
}

void Istemci::hastaListele() {
    komutGonder("hasta_listele");
}

void Istemci::hastaEkle(const QString& tc, const QString& isim, const QString& soyisim,
                        const QString& dogumTarihi, const QString& telefon) {
    QJsonObject veri;
    veri["tc"] = tc;
    veri["isim"] = isim;
    veri["soyisim"] = soyisim;
    veri["dogum_tarihi"] = dogumTarihi;
    veri["telefon"] = telefon;
    komutGonder("hasta_ekle", veri);
}

void Istemci::hastaBul(const QString& tc) {
    QJsonObject veri;
    veri["tc"] = tc;
    komutGonder("hasta_bul", veri);
}

void Istemci::doktorListele() {
    komutGonder("doktor_listele");
}

void Istemci::doktorEkle(const QString& sicil, const QString& isim, const QString& soyisim,
                         const QString& brans, const QStringList& uygunSaatler) {
    QJsonObject veri;
    veri["sicil"] = sicil;
    veri["isim"] = isim;
    veri["soyisim"] = soyisim;
    veri["brans"] = brans;
    
    QJsonArray saatlerArray;
    for (const QString& saat : uygunSaatler) {
        saatlerArray.append(saat);
    }
    veri["uygun_saatler"] = saatlerArray;
    
    komutGonder("doktor_ekle", veri);
}

void Istemci::randevuListele() {
    komutGonder("randevu_listele");
}

void Istemci::randevuOlustur(const QString& tc, const QString& sicil,
                             const QString& tarih, const QString& saat) {
    QJsonObject veri;
    veri["tc"] = tc;
    veri["sicil"] = sicil;
    veri["tarih"] = tarih;
    veri["saat"] = saat;
    komutGonder("randevu_olustur", veri);
}

void Istemci::baglantiKuruldu() {
    emit baglandi();
}

void Istemci::baglantiKesildiFonksiyonu() {
    emit baglantiKoptu();
}

void Istemci::veriOku() {
    QByteArray veri = m_soket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(veri);
    
    if (!doc.isNull() && doc.isObject()) {
        emit yanitAlindi(doc.object());
    }
}

void Istemci::hataOlustu(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    emit hata(m_soket->errorString());
}

void Istemci::komutGonder(const QString& komut, const QJsonObject& veri) {
    if (!bagli()) {
        emit hata("Sunucuya bağlı değil!");
        return;
    }
    
    QJsonObject mesaj;
    mesaj["komut"] = komut;
    mesaj["veri"] = veri;
    
    QJsonDocument doc(mesaj);
    m_soket->write(doc.toJson(QJsonDocument::Compact));
    m_soket->flush();
}
