#include "../include/doktor_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>

DoktorDialog::DoktorDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(tr("Doktor Kayıt Formu"));
    setMinimumWidth(500);
    
    formOlustur();
}

void DoktorDialog::formOlustur() {
    auto* anaLayout = new QVBoxLayout(this);
    
    // Form alanları
    auto* formLayout = new QFormLayout();
    
    m_sicilNoEdit = new QLineEdit(this);
    m_sicilNoEdit->setPlaceholderText("D001");
    formLayout->addRow(tr("Sicil No:"), m_sicilNoEdit);
    
    m_isimEdit = new QLineEdit(this);
    m_isimEdit->setPlaceholderText("İsim");
    formLayout->addRow(tr("İsim:"), m_isimEdit);
    
    m_soyisimEdit = new QLineEdit(this);
    m_soyisimEdit->setPlaceholderText("Soyisim");
    formLayout->addRow(tr("Soyisim:"), m_soyisimEdit);
    
    m_bransComboBox = new QComboBox(this);
    m_bransComboBox->addItems({
        "Dahiliye", "Ortopedi", "Kardiyoloji", "Nöroloji", 
        "Göz Hastalıkları", "KBB", "Genel Cerrahi"
    });
    formLayout->addRow(tr("Branş:"), m_bransComboBox);
    
    anaLayout->addLayout(formLayout);
    
    // Uygun saatler grubu
    auto* saatlerGroup = new QGroupBox(tr("Uygun Saatler"), this);
    auto* saatlerLayout = new QVBoxLayout(saatlerGroup);
    
    m_uygunSaatlerList = new QListWidget(this);
    m_uygunSaatlerList->setMaximumHeight(150);
    saatlerLayout->addWidget(m_uygunSaatlerList);
    
    auto* saatEkleLayout = new QHBoxLayout();
    m_saatComboBox = new QComboBox(this);
    
    // Saat seçenekleri (09:00 - 17:00 arası, 30 dakika aralıklarla)
    for (int saat = 9; saat <= 17; ++saat) {
        m_saatComboBox->addItem(QString("%1:00").arg(saat, 2, 10, QChar('0')));
        if (saat < 17) {
            m_saatComboBox->addItem(QString("%1:30").arg(saat, 2, 10, QChar('0')));
        }
    }
    
    auto* saatEkleBtn = new QPushButton(tr("Ekle"), this);
    auto* saatSilBtn = new QPushButton(tr("Sil"), this);
    
    saatEkleLayout->addWidget(m_saatComboBox);
    saatEkleLayout->addWidget(saatEkleBtn);
    saatEkleLayout->addWidget(saatSilBtn);
    
    saatlerLayout->addLayout(saatEkleLayout);
    anaLayout->addWidget(saatlerGroup);
    
    // Butonlar
    auto* butonLayout = new QHBoxLayout();
    butonLayout->addStretch();
    
    m_tamam = new QPushButton(tr("Tamam"), this);
    m_iptal = new QPushButton(tr("İptal"), this);
    
    butonLayout->addWidget(m_tamam);
    butonLayout->addWidget(m_iptal);
    
    anaLayout->addLayout(butonLayout);
    
    // Sinyal/slot bağlantıları
    connect(m_tamam, &QPushButton::clicked, this, &DoktorDialog::dogrula);
    connect(m_iptal, &QPushButton::clicked, this, &QDialog::reject);
    connect(saatEkleBtn, &QPushButton::clicked, this, &DoktorDialog::saatEkle);
    connect(saatSilBtn, &QPushButton::clicked, this, &DoktorDialog::saatSil);
}

void DoktorDialog::dogrula() {
    if (m_sicilNoEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("Sicil no boş bırakılamaz!"));
        return;
    }
    
    if (m_isimEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("İsim alanı boş bırakılamaz!"));
        return;
    }
    
    if (m_soyisimEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("Soyisim alanı boş bırakılamaz!"));
        return;
    }
    
    if (m_uygunSaatlerList->count() == 0) {
        QMessageBox::warning(this, tr("Hata"), 
            tr("En az bir uygun saat eklemelisiniz!"));
        return;
    }
    
    accept();
}

void DoktorDialog::saatEkle() {
    QString saat = m_saatComboBox->currentText();
    
    // Aynı saat zaten eklenmişse ekleme
    for (int i = 0; i < m_uygunSaatlerList->count(); ++i) {
        if (m_uygunSaatlerList->item(i)->text() == saat) {
            return;
        }
    }
    
    m_uygunSaatlerList->addItem(saat);
    m_uygunSaatlerList->sortItems();
}

void DoktorDialog::saatSil() {
    auto seciliItem = m_uygunSaatlerList->currentItem();
    if (seciliItem) {
        delete seciliItem;
    }
}

Doktor DoktorDialog::doktorAl() const {
    Doktor doktor;
    doktor.sicil_no = m_sicilNoEdit->text().trimmed().toStdString();
    doktor.isim = m_isimEdit->text().trimmed().toStdString();
    doktor.soyisim = m_soyisimEdit->text().trimmed().toStdString();
    doktor.brans = m_bransComboBox->currentText().toStdString();
    
    for (int i = 0; i < m_uygunSaatlerList->count(); ++i) {
        doktor.uygun_saatler.push_back(
            m_uygunSaatlerList->item(i)->text().toStdString());
    }
    
    return doktor;
}

void DoktorDialog::doktorAyarla(const Doktor& doktor) {
    m_sicilNoEdit->setText(QString::fromStdString(doktor.sicil_no));
    m_isimEdit->setText(QString::fromStdString(doktor.isim));
    m_soyisimEdit->setText(QString::fromStdString(doktor.soyisim));
    
    int index = m_bransComboBox->findText(QString::fromStdString(doktor.brans));
    if (index >= 0) {
        m_bransComboBox->setCurrentIndex(index);
    }
    
    m_uygunSaatlerList->clear();
    for (const auto& saat : doktor.uygun_saatler) {
        m_uygunSaatlerList->addItem(QString::fromStdString(saat));
    }
}
