#include "sunucu.h"
#include <QJsonDocument>
#include <QDateTime>

Sunucu::Sunucu(QObject* parent)
    : QTcpServer(parent)
    , m_hastalar(nullptr)
    , m_doktorlar(nullptr)
    , m_randevular(nullptr)
    , m_sonRandevuId(0)
{
}

Sunucu::~Sunucu() {
    durdur();
}

bool Sunucu::basla(quint16 port) {
    if (isListening()) {
        emit hata("Sunucu zaten çalışıyor!");
        return false;
    }
    
    if (!listen(QHostAddress::Any, port)) {
        emit hata("Sunucu başlatılamadı: " + errorString());
        return false;
    }
    
    return true;
}

void Sunucu::durdur() {
    if (isListening()) {
        // Tüm istemcileri kapat
        for (QTcpSocket* istemci : m_istemciler) {
            istemci->disconnectFromHost();
            istemci->deleteLater();
        }
        m_istemciler.clear();
        
        close();
    }
}

void Sunucu::setHastalar(Depo<std::string, Hasta>* hastalar) {
    m_hastalar = hastalar;
}

void Sunucu::setDoktorlar(Depo<std::string, Doktor>* doktorlar) {
    m_doktorlar = doktorlar;
}

void Sunucu::setRandevular(Depo<int, Randevu>* randevular) {
    m_randevular = randevular;
}

void Sunucu::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* istemci = new QTcpSocket(this);
    
    if (!istemci->setSocketDescriptor(socketDescriptor)) {
        delete istemci;
        return;
    }
    
    connect(istemci, &QTcpSocket::readyRead, this, &Sunucu::istemciMesajiOku);
    connect(istemci, &QTcpSocket::disconnected, this, &Sunucu::istemciBaglantisiKoptu);
    
    m_istemciler.append(istemci);
    
    QString adres = istemci->peerAddress().toString() + ":" + 
                    QString::number(istemci->peerPort());
    emit yeniIstemci(adres);
}

void Sunucu::istemciMesajiOku() {
    QTcpSocket* istemci = qobject_cast<QTcpSocket*>(sender());
    if (!istemci) return;
    
    QByteArray veri = istemci->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(veri);
    
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject mesaj = doc.object();
        QString adres = istemci->peerAddress().toString() + ":" + 
                       QString::number(istemci->peerPort());
        emit mesajAlindi(adres, mesaj["komut"].toString());
        
        mesajIsle(istemci, mesaj);
    }
}

void Sunucu::istemciBaglantisiKoptu() {
    QTcpSocket* istemci = qobject_cast<QTcpSocket*>(sender());
    if (!istemci) return;
    
    QString adres = istemci->peerAddress().toString() + ":" + 
                   QString::number(istemci->peerPort());
    emit istemciAyrildi(adres);
    
    m_istemciler.removeOne(istemci);
    istemci->deleteLater();
}

void Sunucu::mesajIsle(QTcpSocket* istemci, const QJsonObject& mesaj) {
    QString komut = mesaj["komut"].toString();
    QJsonObject veri = mesaj["veri"].toObject();
    QJsonObject yanit;
    
    if (komut == "hasta_listele") {
        yanit = hasta_listele();
    } else if (komut == "hasta_ekle") {
        yanit = hasta_ekle(veri);
    } else if (komut == "hasta_bul") {
        yanit = hasta_bul(veri["tc"].toString());
    } else if (komut == "doktor_listele") {
        yanit = doktor_listele();
    } else if (komut == "doktor_ekle") {
        yanit = doktor_ekle(veri);
    } else if (komut == "randevu_listele") {
        yanit = randevu_listele();
    } else if (komut == "randevu_olustur") {
        yanit = randevu_olustur(veri);
    } else {
        yanit["basarili"] = false;
        yanit["hata"] = "Bilinmeyen komut: " + komut;
    }
    
    yanit_gonder(istemci, yanit);
}

void Sunucu::yanit_gonder(QTcpSocket* istemci, const QJsonObject& yanit) {
    QJsonDocument doc(yanit);
    istemci->write(doc.toJson(QJsonDocument::Compact));
    istemci->flush();
}

QJsonObject Sunucu::hasta_listele() {
    QJsonObject yanit;
    
    if (!m_hastalar) {
        yanit["basarili"] = false;
        yanit["hata"] = "Hasta deposu yok";
        return yanit;
    }
    
    QJsonArray hastalar;
    for (const auto& [tc, hasta] : m_hastalar->tumunu_al()) {
        QJsonObject hastaObj;
        hastaObj["tc"] = QString::fromStdString(hasta.tc_no);
        hastaObj["isim"] = QString::fromStdString(hasta.isim);
        hastaObj["soyisim"] = QString::fromStdString(hasta.soyisim);
        hastaObj["dogum_tarihi"] = QString::fromStdString(hasta.dogum_tarihi);
        hastaObj["telefon"] = QString::fromStdString(hasta.telefon);
        hastalar.append(hastaObj);
    }
    
    yanit["basarili"] = true;
    yanit["hastalar"] = hastalar;
    yanit["toplam"] = hastalar.size();
    
    return yanit;
}

QJsonObject Sunucu::hasta_ekle(const QJsonObject& veri) {
    QJsonObject yanit;
    
    if (!m_hastalar) {
        yanit["basarili"] = false;
        yanit["hata"] = "Hasta deposu yok";
        return yanit;
    }
    
    Hasta hasta;
    hasta.tc_no = veri["tc"].toString().toStdString();
    hasta.isim = veri["isim"].toString().toStdString();
    hasta.soyisim = veri["soyisim"].toString().toStdString();
    hasta.dogum_tarihi = veri["dogum_tarihi"].toString().toStdString();
    hasta.telefon = veri["telefon"].toString().toStdString();
    
    if (m_hastalar->ekle(hasta.tc_no, hasta)) {
        yanit["basarili"] = true;
        yanit["mesaj"] = "Hasta başarıyla eklendi";
    } else {
        yanit["basarili"] = false;
        yanit["hata"] = "Hasta eklenemedi (TC no zaten var)";
    }
    
    return yanit;
}

QJsonObject Sunucu::hasta_bul(const QString& tc) {
    QJsonObject yanit;
    
    if (!m_hastalar) {
        yanit["basarili"] = false;
        yanit["hata"] = "Hasta deposu yok";
        return yanit;
    }
    
    auto hasta = m_hastalar->bul(tc.toStdString());
    if (hasta) {
        QJsonObject hastaObj;
        hastaObj["tc"] = QString::fromStdString(hasta->tc_no);
        hastaObj["isim"] = QString::fromStdString(hasta->isim);
        hastaObj["soyisim"] = QString::fromStdString(hasta->soyisim);
        hastaObj["dogum_tarihi"] = QString::fromStdString(hasta->dogum_tarihi);
        hastaObj["telefon"] = QString::fromStdString(hasta->telefon);
        
        yanit["basarili"] = true;
        yanit["hasta"] = hastaObj;
    } else {
        yanit["basarili"] = false;
        yanit["hata"] = "Hasta bulunamadı";
    }
    
    return yanit;
}

QJsonObject Sunucu::doktor_listele() {
    QJsonObject yanit;
    
    if (!m_doktorlar) {
        yanit["basarili"] = false;
        yanit["hata"] = "Doktor deposu yok";
        return yanit;
    }
    
    QJsonArray doktorlar;
    for (const auto& [sicil, doktor] : m_doktorlar->tumunu_al()) {
        QJsonObject doktorObj;
        doktorObj["sicil"] = QString::fromStdString(doktor.sicil_no);
        doktorObj["isim"] = QString::fromStdString(doktor.isim);
        doktorObj["soyisim"] = QString::fromStdString(doktor.soyisim);
        doktorObj["brans"] = QString::fromStdString(doktor.brans);
        
        QJsonArray saatler;
        for (const auto& saat : doktor.uygun_saatler) {
            saatler.append(QString::fromStdString(saat));
        }
        doktorObj["uygun_saatler"] = saatler;
        
        doktorlar.append(doktorObj);
    }
    
    yanit["basarili"] = true;
    yanit["doktorlar"] = doktorlar;
    yanit["toplam"] = doktorlar.size();
    
    return yanit;
}

QJsonObject Sunucu::doktor_ekle(const QJsonObject& veri) {
    QJsonObject yanit;
    
    if (!m_doktorlar) {
        yanit["basarili"] = false;
        yanit["hata"] = "Doktor deposu yok";
        return yanit;
    }
    
    Doktor doktor;
    doktor.sicil_no = veri["sicil"].toString().toStdString();
    doktor.isim = veri["isim"].toString().toStdString();
    doktor.soyisim = veri["soyisim"].toString().toStdString();
    doktor.brans = veri["brans"].toString().toStdString();
    
    QJsonArray saatlerArray = veri["uygun_saatler"].toArray();
    for (const QJsonValue& saat : saatlerArray) {
        doktor.uygun_saatler.push_back(saat.toString().toStdString());
    }
    
    if (m_doktorlar->ekle(doktor.sicil_no, doktor)) {
        yanit["basarili"] = true;
        yanit["mesaj"] = "Doktor başarıyla eklendi";
    } else {
        yanit["basarili"] = false;
        yanit["hata"] = "Doktor eklenemedi (Sicil no zaten var)";
    }
    
    return yanit;
}

QJsonObject Sunucu::randevu_listele() {
    QJsonObject yanit;
    
    if (!m_randevular) {
        yanit["basarili"] = false;
        yanit["hata"] = "Randevu deposu yok";
        return yanit;
    }
    
    QJsonArray randevular;
    for (const auto& [id, randevu] : m_randevular->tumunu_al()) {
        QJsonObject randevuObj;
        randevuObj["id"] = randevu.randevu_id;
        randevuObj["tc"] = QString::fromStdString(randevu.tc_no);
        randevuObj["sicil"] = QString::fromStdString(randevu.sicil_no);
        randevuObj["tarih"] = QString::fromStdString(randevu.tarih);
        randevuObj["saat"] = QString::fromStdString(randevu.saat);
        randevuObj["durum"] = static_cast<int>(randevu.durum);
        
        if (randevu.notlar) {
            randevuObj["notlar"] = QString::fromStdString(*randevu.notlar);
        }
        
        randevular.append(randevuObj);
    }
    
    yanit["basarili"] = true;
    yanit["randevular"] = randevular;
    yanit["toplam"] = randevular.size();
    
    return yanit;
}

QJsonObject Sunucu::randevu_olustur(const QJsonObject& veri) {
    QJsonObject yanit;
    
    if (!m_randevular) {
        yanit["basarili"] = false;
        yanit["hata"] = "Randevu deposu yok";
        return yanit;
    }
    
    Randevu randevu;
    randevu.randevu_id = ++m_sonRandevuId;
    randevu.tc_no = veri["tc"].toString().toStdString();
    randevu.sicil_no = veri["sicil"].toString().toStdString();
    randevu.tarih = veri["tarih"].toString().toStdString();
    randevu.saat = veri["saat"].toString().toStdString();
    randevu.durum = RandevuDurum::Beklemede;
    
    if (m_randevular->ekle(randevu.randevu_id, randevu)) {
        yanit["basarili"] = true;
        yanit["mesaj"] = "Randevu başarıyla oluşturuldu";
        yanit["randevu_id"] = randevu.randevu_id;
    } else {
        yanit["basarili"] = false;
        yanit["hata"] = "Randevu oluşturulamadı";
    }
    
    return yanit;
}
