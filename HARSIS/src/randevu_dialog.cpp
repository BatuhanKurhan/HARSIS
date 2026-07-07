#include "../include/randevu_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDate>

RandevuDialog::RandevuDialog(Depo<std::string, Hasta>* hastalar,
                             Depo<std::string, Doktor>* doktorlar,
                             QWidget* parent)
    : QDialog(parent), m_hastalar(hastalar), m_doktorlar(doktorlar), m_randevuId(0) {
    setWindowTitle(tr("Randevu Oluştur"));
    setMinimumWidth(450);
    
    formOlustur();
    hastalariYukle();
    branslariYukle();
}

void RandevuDialog::formOlustur() {
    auto* anaLayout = new QVBoxLayout(this);
    
    // Form alanları
    auto* formLayout = new QFormLayout();
    
    m_hastaComboBox = new QComboBox(this);
    formLayout->addRow(tr("Hasta:"), m_hastaComboBox);
    
    m_bransComboBox = new QComboBox(this);
    formLayout->addRow(tr("Branş:"), m_bransComboBox);
    
    m_doktorComboBox = new QComboBox(this);
    formLayout->addRow(tr("Doktor:"), m_doktorComboBox);
    
    m_tarihEdit = new QDateEdit(this);
    m_tarihEdit->setCalendarPopup(true);
    m_tarihEdit->setDate(QDate::currentDate());
    m_tarihEdit->setDisplayFormat("dd-MM-yyyy");
    m_tarihEdit->setMinimumDate(QDate::currentDate());
    formLayout->addRow(tr("Tarih:"), m_tarihEdit);
    
    m_saatComboBox = new QComboBox(this);
    formLayout->addRow(tr("Saat:"), m_saatComboBox);
    
    anaLayout->addLayout(formLayout);
    
    // Butonlar
    auto* butonLayout = new QHBoxLayout();
    butonLayout->addStretch();
    
    m_tamam = new QPushButton(tr("Randevu Oluştur"), this);
    m_iptal = new QPushButton(tr("İptal"), this);
    
    butonLayout->addWidget(m_tamam);
    butonLayout->addWidget(m_iptal);
    
    anaLayout->addLayout(butonLayout);
    
    // Sinyal/slot bağlantıları
    connect(m_tamam, &QPushButton::clicked, this, &RandevuDialog::dogrula);
    connect(m_iptal, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_bransComboBox, &QComboBox::currentTextChanged, 
            this, &RandevuDialog::bransSecildi);
    connect(m_doktorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RandevuDialog::doktorSecildi);
}

void RandevuDialog::hastalariYukle() {
    m_hastaComboBox->clear();
    
    for (const auto& [tc, hasta] : m_hastalar->tumunu_al()) {
        QString metin = QString("%1 - %2 %3")
            .arg(QString::fromStdString(hasta.tc_no))
            .arg(QString::fromStdString(hasta.isim))
            .arg(QString::fromStdString(hasta.soyisim));
        m_hastaComboBox->addItem(metin, QString::fromStdString(hasta.tc_no));
    }
}

void RandevuDialog::branslariYukle() {
    m_bransComboBox->clear();
    
    std::set<std::string> branslar;
    for (const auto& [sicil, doktor] : m_doktorlar->tumunu_al()) {
        branslar.insert(doktor.brans);
    }
    
    for (const auto& brans : branslar) {
        m_bransComboBox->addItem(QString::fromStdString(brans));
    }
    
    if (m_bransComboBox->count() > 0) {
        bransSecildi(m_bransComboBox->currentText());
    }
}

void RandevuDialog::doktorlariYukle(const QString& brans) {
    m_doktorComboBox->clear();
    m_saatComboBox->clear();
    
    for (const auto& [sicil, doktor] : m_doktorlar->tumunu_al()) {
        if (QString::fromStdString(doktor.brans) == brans) {
            QString metin = QString("Dr. %1 %2 (%3)")
                .arg(QString::fromStdString(doktor.isim))
                .arg(QString::fromStdString(doktor.soyisim))
                .arg(QString::fromStdString(doktor.sicil_no));
            m_doktorComboBox->addItem(metin, QString::fromStdString(doktor.sicil_no));
        }
    }
    
    if (m_doktorComboBox->count() > 0) {
        doktorSecildi(0);
    }
}

void RandevuDialog::uygunSaatleriYukle(const QString& sicilNo) {
    m_saatComboBox->clear();
    
    auto doktor = m_doktorlar->bul(sicilNo.toStdString());
    if (doktor.has_value()) {
        for (const auto& saat : doktor->uygun_saatler) {
            m_saatComboBox->addItem(QString::fromStdString(saat));
        }
    }
}

void RandevuDialog::bransSecildi(const QString& brans) {
    doktorlariYukle(brans);
}

void RandevuDialog::doktorSecildi(int index) {
    if (index >= 0 && index < m_doktorComboBox->count()) {
        QString sicilNo = m_doktorComboBox->itemData(index).toString();
        uygunSaatleriYukle(sicilNo);
    }
}

void RandevuDialog::dogrula() {
    if (m_hastaComboBox->count() == 0) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("Sistemde kayıtlı hasta bulunmamaktadır!"));
        return;
    }
    
    if (m_doktorComboBox->count() == 0) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("Seçilen branşta doktor bulunmamaktadır!"));
        return;
    }
    
    if (m_saatComboBox->count() == 0) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("Seçilen doktorun uygun saati bulunmamaktadır!"));
        return;
    }
    
    accept();
}

Randevu RandevuDialog::randevuAl() const {
    Randevu randevu;
    randevu.randevu_id = m_randevuId;
    randevu.tc_no = m_hastaComboBox->currentData().toString().toStdString();
    randevu.sicil_no = m_doktorComboBox->currentData().toString().toStdString();
    randevu.tarih = m_tarihEdit->date().toString("yyyy-MM-dd").toStdString();
    randevu.saat = m_saatComboBox->currentText().toStdString();
    randevu.durum = RandevuDurum::Beklemede;
    randevu.notlar = std::nullopt;
    return randevu;
}

void RandevuDialog::randevuIdAyarla(int id) {
    m_randevuId = id;
}
