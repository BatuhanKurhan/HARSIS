#include "../include/hasta_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDate>

HastaDialog::HastaDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(tr("Hasta Kayıt Formu"));
    setMinimumWidth(400);
    
    formOlustur();
}

void HastaDialog::formOlustur() {
    auto* anaLayout = new QVBoxLayout(this);
    
    // Form alanları
    auto* formLayout = new QFormLayout();
    
    m_tcNoEdit = new QLineEdit(this);
    m_tcNoEdit->setMaxLength(11);
    m_tcNoEdit->setPlaceholderText("11 haneli TC kimlik no");
    formLayout->addRow(tr("TC Kimlik No:"), m_tcNoEdit);
    
    m_isimEdit = new QLineEdit(this);
    m_isimEdit->setPlaceholderText("İsim");
    formLayout->addRow(tr("İsim:"), m_isimEdit);
    
    m_soyisimEdit = new QLineEdit(this);
    m_soyisimEdit->setPlaceholderText("Soyisim");
    formLayout->addRow(tr("Soyisim:"), m_soyisimEdit);
    
    m_dogumTarihiEdit = new QDateEdit(this);
    m_dogumTarihiEdit->setCalendarPopup(true);
    m_dogumTarihiEdit->setDate(QDate(1990, 1, 1));
    m_dogumTarihiEdit->setDisplayFormat("dd-MM-yyyy");
    formLayout->addRow(tr("Doğum Tarihi:"), m_dogumTarihiEdit);
    
    m_telefonEdit = new QLineEdit(this);
    m_telefonEdit->setPlaceholderText("555-0123");
    formLayout->addRow(tr("Telefon:"), m_telefonEdit);
    
    anaLayout->addLayout(formLayout);
    
    // Butonlar
    auto* butonLayout = new QHBoxLayout();
    butonLayout->addStretch();
    
    m_tamam = new QPushButton(tr("Tamam"), this);
    m_iptal = new QPushButton(tr("İptal"), this);
    
    butonLayout->addWidget(m_tamam);
    butonLayout->addWidget(m_iptal);
    
    anaLayout->addLayout(butonLayout);
    
    // Sinyal/slot bağlantıları
    connect(m_tamam, &QPushButton::clicked, this, &HastaDialog::dogrula);
    connect(m_iptal, &QPushButton::clicked, this, &QDialog::reject);
}

void HastaDialog::dogrula() {
    // TC no kontrolü
    if (m_tcNoEdit->text().length() != 11) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("TC kimlik numarası 11 haneli olmalıdır!"));
        return;
    }
    
    // İsim kontrolü
    if (m_isimEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("İsim alanı boş bırakılamaz!"));
        return;
    }
    
    // Soyisim kontrolü
    if (m_soyisimEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("Soyisim alanı boş bırakılamaz!"));
        return;
    }
    
    accept();
}

Hasta HastaDialog::hastaAl() const {
    Hasta hasta;
    hasta.tc_no = m_tcNoEdit->text().toStdString();
    hasta.isim = m_isimEdit->text().trimmed().toStdString();
    hasta.soyisim = m_soyisimEdit->text().trimmed().toStdString();
    hasta.dogum_tarihi = m_dogumTarihiEdit->date().toString("yyyy-MM-dd").toStdString();
    hasta.telefon = m_telefonEdit->text().toStdString();
    return hasta;
}

void HastaDialog::hastaAyarla(const Hasta& hasta) {
    m_tcNoEdit->setText(QString::fromStdString(hasta.tc_no));
    m_isimEdit->setText(QString::fromStdString(hasta.isim));
    m_soyisimEdit->setText(QString::fromStdString(hasta.soyisim));
    m_dogumTarihiEdit->setDate(QDate::fromString(
        QString::fromStdString(hasta.dogum_tarihi), "yyyy-MM-dd"));
    m_telefonEdit->setText(QString::fromStdString(hasta.telefon));
}
