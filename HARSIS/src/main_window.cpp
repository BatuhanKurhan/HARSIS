#include "../include/main_window.h"
#include "../include/hasta_dialog.h"
#include "../include/doktor_dialog.h"
#include "../include/randevu_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QGroupBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QTextEdit>
#include <fstream>
#include <set>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_sonRandevuId(0), m_ilerlemeDiyalogu(nullptr) {
    
    setWindowTitle(tr("HARSIS - Hastane Randevu Sistemi"));
    setMinimumSize(1000, 700);
    
    // Veri depolarını oluştur
    m_hastalar = std::make_unique<Depo<std::string, Hasta>>();
    m_doktorlar = std::make_unique<Depo<std::string, Doktor>>();
    m_randevular = std::make_unique<Depo<int, Randevu>>();
    
    // Örnek verileri yükle
    ornekVerilerYukle();
    
    // UI bileşenlerini oluştur
    menuOlustur();
    aracCubuguOlustur();
    sekmeliGorunumOlustur();
    
    // Faz 3: Thread servislerini başlat
    m_hatirlatmaServisi = new HatirlatmaServisi(m_randevular.get(), this);
    m_raporUretici = new RaporUretici(m_randevular.get(), m_doktorlar.get(), this);
    m_csvAktarici = new CsvAktarici(m_hastalar.get(), this);
    
    // Faz 4: Sunucuyu oluştur
    m_sunucu = new Sunucu(this);
    m_sunucu->setHastalar(m_hastalar.get());
    m_sunucu->setDoktorlar(m_doktorlar.get());
    m_sunucu->setRandevular(m_randevular.get());
    
    // Thread sinyallerini bağla
    connect(m_hatirlatmaServisi, &HatirlatmaServisi::hatirlatmaOlustu,
            this, &MainWindow::hatirlatmaGeldi);
    connect(m_raporUretici, &RaporUretici::raporHazir,
            this, &MainWindow::raporHazir);
    connect(m_csvAktarici, &CsvAktarici::ilerlemeDegisti,
            this, &MainWindow::csvIlerlemeDegisti);
    connect(m_csvAktarici, &CsvAktarici::aktarimTamamlandi,
            this, &MainWindow::csvAktarimTamamlandi);
    
    // Ayarları yükle (pencere boyutu/konumu)
    ayarlariYukle();
    
    // Durum çubuğunu güncelle
    durumCubuguGuncelle();
}

MainWindow::~MainWindow() {
    // Faz 4: Sunucuyu durdur
    if (m_sunucu) {
        m_sunucu->durdur();
    }
    
    // Thread'leri durdur
    if (m_hatirlatmaServisi) {
        m_hatirlatmaServisi->durdur();
        m_hatirlatmaServisi->wait();
    }
    if (m_raporUretici) {
        m_raporUretici->iptalEt();
        m_raporUretici->wait();
    }
    if (m_csvAktarici) {
        m_csvAktarici->iptalEt();
        m_csvAktarici->wait();
    }
}

void MainWindow::menuOlustur() {
    // Dosya menüsü
    m_dosyaMenu = menuBar()->addMenu(tr("&Dosya"));
    
    auto* kaydetAksiyon = m_dosyaMenu->addAction(tr("&Kaydet"));
    kaydetAksiyon->setShortcut(QKeySequence::Save);
    connect(kaydetAksiyon, &QAction::triggered, this, &MainWindow::dosyaKaydet);
    
    auto* yukleAksiyon = m_dosyaMenu->addAction(tr("&Yükle"));
    kaydetAksiyon->setShortcut(QKeySequence::Open);
    connect(yukleAksiyon, &QAction::triggered, this, &MainWindow::dosyaYukle);
    
    m_dosyaMenu->addSeparator();
    
    auto* cikisAksiyon = m_dosyaMenu->addAction(tr("Çı&kış"));
    cikisAksiyon->setShortcut(QKeySequence::Quit);
    connect(cikisAksiyon, &QAction::triggered, this, &QWidget::close);
    
    // Düzen menüsü
    m_duzenMenu = menuBar()->addMenu(tr("&Düzen"));
    
    auto* hastaEkleAksiyon = m_duzenMenu->addAction(tr("&Hasta Ekle"));
    connect(hastaEkleAksiyon, &QAction::triggered, this, &MainWindow::hastaEkle);
    
    auto* doktorEkleAksiyon = m_duzenMenu->addAction(tr("&Doktor Ekle"));
    connect(doktorEkleAksiyon, &QAction::triggered, this, &MainWindow::doktorEkle);
    
    auto* randevuAlAksiyon = m_duzenMenu->addAction(tr("&Randevu Al"));
    connect(randevuAlAksiyon, &QAction::triggered, this, &MainWindow::randevuAl);
    
    // Yardım menüsü
    m_yardimMenu = menuBar()->addMenu(tr("&Yardım"));
    
    auto* hakkindaAksiyon = m_yardimMenu->addAction(tr("&Hakkında"));
    connect(hakkindaAksiyon, &QAction::triggered, this, &MainWindow::hakkinda);
    
    // Faz 3: Servisler menüsü
    m_servislerMenu = menuBar()->addMenu(tr("&Servisler"));
    
    auto* hatirlatmaBaslatAksiyon = m_servislerMenu->addAction(tr("Hatırlatma Servisini &Başlat"));
    connect(hatirlatmaBaslatAksiyon, &QAction::triggered, this, &MainWindow::hatirlatmaServisiniBaslat);
    
    auto* hatirlatmaDurdurAksiyon = m_servislerMenu->addAction(tr("Hatırlatma Servisini &Durdur"));
    connect(hatirlatmaDurdurAksiyon, &QAction::triggered, this, &MainWindow::hatirlatmaServisiniDurdur);
    
    m_servislerMenu->addSeparator();
    
    auto* raporUretAksiyon = m_servislerMenu->addAction(tr("&Rapor Üret"));
    connect(raporUretAksiyon, &QAction::triggered, this, &MainWindow::raporUret);
    
    m_servislerMenu->addSeparator();
    
    auto* csvIceAktarAksiyon = m_servislerMenu->addAction(tr("CSV'den &İçe Aktar"));
    connect(csvIceAktarAksiyon, &QAction::triggered, this, &MainWindow::csvIceAktar);
    
    auto* csvDisaAktarAksiyon = m_servislerMenu->addAction(tr("CSV'ye &Dışa Aktar"));
    connect(csvDisaAktarAksiyon, &QAction::triggered, this, &MainWindow::csvDisaAktar);
    
    // Faz 4: Ağ menüsü
    m_agMenu = menuBar()->addMenu(tr("&Ağ"));
    
    auto* sunucuBaslatAksiyon = m_agMenu->addAction(tr("&Sunucu Başlat"));
    connect(sunucuBaslatAksiyon, &QAction::triggered, this, &MainWindow::sunucuBaslat);
    
    auto* sunucuDurdurAksiyon = m_agMenu->addAction(tr("Sunucu &Durdur"));
    connect(sunucuDurdurAksiyon, &QAction::triggered, this, &MainWindow::sunucuDurdur);
}

void MainWindow::aracCubuguOlustur() {
    m_aracCubugu = addToolBar(tr("Ana Araç Çubuğu"));
    m_aracCubugu->setMovable(false);
    
    auto* hastaEkleBtn = m_aracCubugu->addAction(tr("Hasta Ekle"));
    connect(hastaEkleBtn, &QAction::triggered, this, &MainWindow::hastaEkle);
    
    auto* doktorEkleBtn = m_aracCubugu->addAction(tr("Doktor Ekle"));
    connect(doktorEkleBtn, &QAction::triggered, this, &MainWindow::doktorEkle);
    
    auto* randevuAlBtn = m_aracCubugu->addAction(tr("Randevu Al"));
    connect(randevuAlBtn, &QAction::triggered, this, &MainWindow::randevuAl);
    
    m_aracCubugu->addSeparator();
    
    auto* kaydetBtn = m_aracCubugu->addAction(tr("Kaydet"));
    connect(kaydetBtn, &QAction::triggered, this, &MainWindow::dosyaKaydet);
}

void MainWindow::sekmeliGorunumOlustur() {
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    
    // === HASTA SEKMESİ ===
    auto* hastaWidget = new QWidget();
    auto* hastaLayout = new QVBoxLayout(hastaWidget);
    
    // Arama kutusu
    auto* hastaAramaLayout = new QHBoxLayout();
    hastaAramaLayout->addWidget(new QLabel(tr("Ara:")));
    m_hastaAramaKutusu = new QLineEdit();
    m_hastaAramaKutusu->setPlaceholderText(tr("TC, isim veya soyisim ile ara..."));
    hastaAramaLayout->addWidget(m_hastaAramaKutusu);
    hastaLayout->addLayout(hastaAramaLayout);
    
    // Tablo
    m_hastaTablosu = new QTableView();
    m_hastaModel = new HastaModel(m_hastalar.get(), this);
    m_hastaProxyModel = new QSortFilterProxyModel(this);
    m_hastaProxyModel->setSourceModel(m_hastaModel);
    m_hastaProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_hastaProxyModel->setFilterKeyColumn(-1); // Tüm sütunlarda ara
    m_hastaTablosu->setModel(m_hastaProxyModel);
    m_hastaTablosu->setSortingEnabled(true);
    m_hastaTablosu->horizontalHeader()->setStretchLastSection(true);
    hastaLayout->addWidget(m_hastaTablosu);
    
    connect(m_hastaAramaKutusu, &QLineEdit::textChanged, 
            this, &MainWindow::hastaAra);
    
    m_tabWidget->addTab(hastaWidget, tr("Hastalar"));
    
    // === DOKTOR SEKMESİ ===
    auto* doktorWidget = new QWidget();
    auto* doktorLayout = new QVBoxLayout(doktorWidget);
    
    // Filtreleme
    auto* doktorFiltreLayout = new QHBoxLayout();
    doktorFiltreLayout->addWidget(new QLabel(tr("Branş:")));
    m_bransComboBox = new QComboBox();
    m_bransComboBox->addItem(tr("Tümü"));
    doktorFiltreLayout->addWidget(m_bransComboBox);
    doktorFiltreLayout->addWidget(new QLabel(tr("Ara:")));
    m_doktorAramaKutusu = new QLineEdit();
    m_doktorAramaKutusu->setPlaceholderText(tr("Sicil, isim veya soyisim ile ara..."));
    doktorFiltreLayout->addWidget(m_doktorAramaKutusu);
    doktorLayout->addLayout(doktorFiltreLayout);
    
    // Tablo
    m_doktorTablosu = new QTableView();
    m_doktorModel = new DoktorModel(m_doktorlar.get(), this);
    m_doktorProxyModel = new QSortFilterProxyModel(this);
    m_doktorProxyModel->setSourceModel(m_doktorModel);
    m_doktorProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_doktorProxyModel->setFilterKeyColumn(-1);
    m_doktorTablosu->setModel(m_doktorProxyModel);
    m_doktorTablosu->setSortingEnabled(true);
    m_doktorTablosu->horizontalHeader()->setStretchLastSection(true);
    doktorLayout->addWidget(m_doktorTablosu);
    
    connect(m_doktorAramaKutusu, &QLineEdit::textChanged, 
            this, &MainWindow::doktorAra);
    connect(m_bransComboBox, &QComboBox::currentTextChanged,
            this, &MainWindow::bransFiltrele);
    
    m_tabWidget->addTab(doktorWidget, tr("Doktorlar"));
    
    // === RANDEVU SEKMESİ ===
    auto* randevuWidget = new QWidget();
    auto* randevuLayout = new QVBoxLayout(randevuWidget);
    
    // Arama kutusu
    auto* randevuAramaLayout = new QHBoxLayout();
    randevuAramaLayout->addWidget(new QLabel(tr("Ara:")));
    m_randevuAramaKutusu = new QLineEdit();
    m_randevuAramaKutusu->setPlaceholderText(tr("TC, sicil veya tarih ile ara..."));
    randevuAramaLayout->addWidget(m_randevuAramaKutusu);
    randevuLayout->addLayout(randevuAramaLayout);
    
    // Tablo
    m_randevuTablosu = new QTableView();
    m_randevuModel = new RandevuModel(m_randevular.get(), this);
    m_randevuProxyModel = new QSortFilterProxyModel(this);
    m_randevuProxyModel->setSourceModel(m_randevuModel);
    m_randevuProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_randevuProxyModel->setFilterKeyColumn(-1);
    m_randevuTablosu->setModel(m_randevuProxyModel);
    m_randevuTablosu->setSortingEnabled(true);
    m_randevuTablosu->horizontalHeader()->setStretchLastSection(true);
    randevuLayout->addWidget(m_randevuTablosu);
    
    connect(m_randevuAramaKutusu, &QLineEdit::textChanged, 
            this, &MainWindow::randevuAra);
    
    m_tabWidget->addTab(randevuWidget, tr("Randevular"));
    
    // Branş combobox'ını güncelle
    std::set<std::string> branslar;
    for (const auto& [sicil, doktor] : m_doktorlar->tumunu_al()) {
        branslar.insert(doktor.brans);
    }
    for (const auto& brans : branslar) {
        m_bransComboBox->addItem(QString::fromStdString(brans));
    }
}

void MainWindow::hastaEkle() {
    HastaDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Hasta hasta = dialog.hastaAl();
        
        if (m_hastalar->bul(hasta.tc_no).has_value()) {
            QMessageBox::warning(this, tr("Hata"), 
                tr("Bu TC kimlik numarasına sahip hasta zaten kayıtlı!"));
            return;
        }
        
        m_hastalar->ekle(hasta.tc_no, hasta);
        m_hastaModel->yenile();
        durumCubuguGuncelle();
        
        QMessageBox::information(this, tr("Başarılı"), 
            tr("Hasta başarıyla eklendi."));
    }
}

void MainWindow::doktorEkle() {
    DoktorDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Doktor doktor = dialog.doktorAl();
        
        if (m_doktorlar->bul(doktor.sicil_no).has_value()) {
            QMessageBox::warning(this, tr("Hata"), 
                tr("Bu sicil numarasına sahip doktor zaten kayıtlı!"));
            return;
        }
        
        m_doktorlar->ekle(doktor.sicil_no, doktor);
        m_doktorModel->yenile();
        
        // Branş combobox'ını güncelle
        QString brans = QString::fromStdString(doktor.brans);
        if (m_bransComboBox->findText(brans) == -1) {
            m_bransComboBox->addItem(brans);
        }
        
        durumCubuguGuncelle();
        
        QMessageBox::information(this, tr("Başarılı"), 
            tr("Doktor başarıyla eklendi."));
    }
}

void MainWindow::randevuAl() {
    RandevuDialog dialog(m_hastalar.get(), m_doktorlar.get(), this);
    dialog.randevuIdAyarla(++m_sonRandevuId);
    
    if (dialog.exec() == QDialog::Accepted) {
        Randevu randevu = dialog.randevuAl();
        
        // Çakışma kontrolü
        auto cakisan = m_randevular->filtrele([&](const Randevu& r) {
            return r.sicil_no == randevu.sicil_no &&
                   r.tarih == randevu.tarih &&
                   r.saat == randevu.saat &&
                   r.durum != RandevuDurum::Iptal;
        });
        
        if (!cakisan.empty()) {
            QMessageBox::warning(this, tr("Hata"), 
                tr("Bu doktorun seçilen tarih ve saatte başka randevusu var!"));
            --m_sonRandevuId;
            return;
        }
        
        m_randevular->ekle(randevu.randevu_id, randevu);
        m_randevuModel->yenile();
        durumCubuguGuncelle();
        
        QMessageBox::information(this, tr("Başarılı"), 
            tr("Randevu başarıyla oluşturuldu.\nRandevu ID: %1").arg(randevu.randevu_id));
    }
}

void MainWindow::dosyaKaydet() {
    // Basit binary kaydetme (Faz 1'deki gibi)
    std::ofstream dosya("harsis_data.dat", std::ios::binary);
    if (!dosya) {
        QMessageBox::critical(this, tr("Hata"), 
            tr("Dosya kaydedilemedi!"));
        return;
    }
    
    // Hasta sayısı
    size_t hasta_sayisi = m_hastalar->boyut();
    dosya.write(reinterpret_cast<const char*>(&hasta_sayisi), sizeof(hasta_sayisi));
    
    // Doktor sayısı
    size_t doktor_sayisi = m_doktorlar->boyut();
    dosya.write(reinterpret_cast<const char*>(&doktor_sayisi), sizeof(doktor_sayisi));
    
    // Randevu sayısı
    size_t randevu_sayisi = m_randevular->boyut();
    dosya.write(reinterpret_cast<const char*>(&randevu_sayisi), sizeof(randevu_sayisi));
    
    dosya.write(reinterpret_cast<const char*>(&m_sonRandevuId), sizeof(m_sonRandevuId));
    
    statusBar()->showMessage(tr("Veriler kaydedildi: harsis_data.dat"), 3000);
}

void MainWindow::dosyaYukle() {
    std::ifstream dosya("harsis_data.dat", std::ios::binary);
    if (!dosya) {
        QMessageBox::warning(this, tr("Uyarı"), 
            tr("Kaydedilmiş veri dosyası bulunamadı!"));
        return;
    }
    
    statusBar()->showMessage(tr("Veriler yüklendi: harsis_data.dat"), 3000);
}

void MainWindow::hakkinda() {
    QMessageBox::about(this, tr("HARSIS Hakkında"),
        tr("<h2>HARSIS - Hastane Randevu Sistemi</h2>"
           "<p>Versiyon: 2.0 (Faz 2)</p>"
           "<p>Qt6 ile geliştirilmiş hastane randevu yönetim sistemi.</p>"
           "<p><b>Özellikler:</b></p>"
           "<ul>"
           "<li>Hasta kaydı ve yönetimi</li>"
           "<li>Doktor kaydı ve branş yönetimi</li>"
           "<li>Randevu oluşturma ve takip</li>"
           "<li>Arama ve filtreleme</li>"
           "<li>Veri kaydetme/yükleme</li>"
           "</ul>"
           "<p>Yazılım Geliştirme II - Proje Ödevi</p>"));
}

void MainWindow::hastaAra(const QString& metin) {
    m_hastaProxyModel->setFilterFixedString(metin);
}

void MainWindow::doktorAra(const QString& metin) {
    m_doktorProxyModel->setFilterFixedString(metin);
}

void MainWindow::randevuAra(const QString& metin) {
    m_randevuProxyModel->setFilterFixedString(metin);
}

void MainWindow::bransFiltrele(const QString& brans) {
    if (brans == tr("Tümü")) {
        m_doktorProxyModel->setFilterFixedString("");
    } else {
        m_doktorProxyModel->setFilterKeyColumn(3); // Branş sütunu
        m_doktorProxyModel->setFilterFixedString(brans);
        m_doktorProxyModel->setFilterKeyColumn(-1); // Tekrar tüm sütunlar
    }
}

void MainWindow::durumCubuguGuncelle() {
    QString mesaj = tr("Hastalar: %1 | Doktorlar: %2 | Randevular: %3")
        .arg(m_hastalar->boyut())
        .arg(m_doktorlar->boyut())
        .arg(m_randevular->boyut());
    statusBar()->showMessage(mesaj);
}

void MainWindow::ayarlariKaydet() {
    QSettings ayarlar("HARSIS", "HastaneRandevuSistemi");
    ayarlar.setValue("pencere/boyut", size());
    ayarlar.setValue("pencere/konum", pos());
}

void MainWindow::ayarlariYukle() {
    QSettings ayarlar("HARSIS", "HastaneRandevuSistemi");
    
    if (ayarlar.contains("pencere/boyut")) {
        resize(ayarlar.value("pencere/boyut").toSize());
    }
    
    if (ayarlar.contains("pencere/konum")) {
        move(ayarlar.value("pencere/konum").toPoint());
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    ayarlariKaydet();
    event->accept();
}

void MainWindow::ornekVerilerYukle() {
    // Örnek hastalar
    m_hastalar->ekle("12345678901", 
        {"12345678901", "Ali", "Yilmaz", "1985-03-15", "555-0101"});
    m_hastalar->ekle("23456789012", 
        {"23456789012", "Zeynep", "Kara", "1990-07-22", "555-0102"});
    m_hastalar->ekle("34567890123", 
        {"34567890123", "Mehmet", "Demir", "1978-11-05", "555-0103"});
    
    // Örnek doktorlar
    m_doktorlar->ekle("D001", 
        {"D001", "Ayse", "Ozturk", "Dahiliye", 
         {"09:00", "09:30", "10:00", "10:30", "11:00"}});
    m_doktorlar->ekle("D002", 
        {"D002", "Hasan", "Celik", "Ortopedi", 
         {"13:00", "13:30", "14:00", "14:30"}});
    m_doktorlar->ekle("D003", 
        {"D003", "Fatma", "Sahin", "Dahiliye", 
         {"09:00", "09:30", "10:00", "14:00", "14:30"}});
    
    // Örnek randevular
    m_randevular->ekle(1, 
        {1, "12345678901", "D001", "2026-05-20", "09:00", 
         RandevuDurum::Onaylandi, "Kontrol muayenesi"});
    m_randevular->ekle(2, 
        {2, "23456789012", "D002", "2026-05-20", "13:30", 
         RandevuDurum::Beklemede, std::nullopt});
    m_randevular->ekle(3, 
        {3, "12345678901", "D003", "2026-05-21", "10:00", 
         RandevuDurum::Tamamlandi, "Kan tahlili sonuclari normal"});
    
    m_sonRandevuId = 3;
}


// ============================================================================
// FAZ 3: ÇOK KANALLI PROGRAMLAMA FONKSİYONLARI
// ============================================================================

void MainWindow::hatirlatmaServisiniBaslat() {
    if (m_hatirlatmaServisi->isRunning()) {
        QMessageBox::information(this, tr("Bilgi"), 
            tr("Hatırlatma servisi zaten çalışıyor."));
        return;
    }
    
    bool ok;
    int aralik = QInputDialog::getInt(this, tr("Tarama Aralığı"),
        tr("Tarama aralığını saniye cinsinden girin:"),
        60, 10, 3600, 10, &ok);
    
    if (ok) {
        m_hatirlatmaServisi->taramaAraliginiAyarla(aralik);
        m_hatirlatmaServisi->start();
        statusBar()->showMessage(tr("Hatırlatma servisi başlatıldı (Aralık: %1 saniye)").arg(aralik), 5000);
    }
}

void MainWindow::hatirlatmaServisiniDurdur() {
    if (!m_hatirlatmaServisi->isRunning()) {
        QMessageBox::information(this, tr("Bilgi"), 
            tr("Hatırlatma servisi zaten durmuş durumda."));
        return;
    }
    
    m_hatirlatmaServisi->durdur();
    m_hatirlatmaServisi->wait();
    statusBar()->showMessage(tr("Hatırlatma servisi durduruldu"), 3000);
}

void MainWindow::hatirlatmaGeldi(const QString& mesaj) {
    QMessageBox::information(this, tr("Randevu Hatırlatması"), mesaj);
}

void MainWindow::raporUret() {
    if (m_raporUretici->isRunning()) {
        QMessageBox::warning(this, tr("Uyarı"), 
            tr("Bir rapor zaten oluşturuluyor. Lütfen bekleyin."));
        return;
    }
    
    // Tarih aralığı sor
    bool ok;
    QString baslangic = QInputDialog::getText(this, tr("Başlangıç Tarihi"),
        tr("Başlangıç tarihi (DD-MM-YYYY):"), QLineEdit::Normal,
        "01-01-2026", &ok);
    
    if (!ok) return;
    
    QString bitis = QInputDialog::getText(this, tr("Bitiş Tarihi"),
        tr("Bitiş tarihi (DD-MM-YYYY):"), QLineEdit::Normal,
        "31-12-2026", &ok);
    
    if (!ok) return;
    
    // İlerleme dialogu oluştur
    m_ilerlemeDiyalogu = new QProgressDialog(tr("Rapor oluşturuluyor..."), 
        tr("İptal"), 0, 100, this);
    m_ilerlemeDiyalogu->setWindowModality(Qt::WindowModal);
    m_ilerlemeDiyalogu->setMinimumDuration(0);
    
    connect(m_raporUretici, &RaporUretici::ilerlemeDegisti,
            m_ilerlemeDiyalogu, &QProgressDialog::setValue);
    connect(m_ilerlemeDiyalogu, &QProgressDialog::canceled,
            m_raporUretici, &RaporUretici::iptalEt);
    
    m_raporUretici->raporUret(baslangic, bitis);
}

void MainWindow::raporHazir(const RaporSonucu& sonuc) {
    if (m_ilerlemeDiyalogu) {
        m_ilerlemeDiyalogu->close();
        delete m_ilerlemeDiyalogu;
        m_ilerlemeDiyalogu = nullptr;
    }
    
    // Rapor sonuçlarını göster
    QString raporMetni = QString(
        "<h2>Randevu Raporu</h2>"
        "<p><b>Oluşturma Tarihi:</b> %1</p>"
        "<hr>"
        "<h3>Genel İstatistikler</h3>"
        "<table border='1' cellpadding='5'>"
        "<tr><td><b>Toplam Randevu:</b></td><td>%2</td></tr>"
        "<tr><td><b>Onaylanan:</b></td><td>%3</td></tr>"
        "<tr><td><b>Bekleyen:</b></td><td>%4</td></tr>"
        "<tr><td><b>İptal Edilen:</b></td><td>%5</td></tr>"
        "<tr><td><b>Tamamlanan:</b></td><td>%6</td></tr>"
        "</table>"
        "<hr>"
        "<h3>Branşa Göre Dağılım</h3>"
        "<table border='1' cellpadding='5'>"
        "<tr><th>Branş</th><th>Randevu Sayısı</th></tr>")
        .arg(sonuc.olusturmaTarihi)
        .arg(sonuc.toplamRandevu)
        .arg(sonuc.onaylananRandevu)
        .arg(sonuc.bekleyenRandevu)
        .arg(sonuc.iptalEdilenRandevu)
        .arg(sonuc.tamamlananRandevu);
    
    for (const auto& [brans, sayi] : sonuc.bransaBaziDagilim) {
        raporMetni += QString("<tr><td>%1</td><td>%2</td></tr>")
            .arg(QString::fromStdString(brans))
            .arg(sayi);
    }
    
    raporMetni += "</table>";
    
    // Raporu göster
    QDialog* raporDialog = new QDialog(this);
    raporDialog->setWindowTitle(tr("Randevu Raporu"));
    raporDialog->resize(600, 500);
    
    auto* layout = new QVBoxLayout(raporDialog);
    auto* textEdit = new QTextEdit();
    textEdit->setHtml(raporMetni);
    textEdit->setReadOnly(true);
    layout->addWidget(textEdit);
    
    auto* kapatBtn = new QPushButton(tr("Kapat"));
    connect(kapatBtn, &QPushButton::clicked, raporDialog, &QDialog::accept);
    layout->addWidget(kapatBtn);
    
    raporDialog->exec();
    delete raporDialog;
}

void MainWindow::csvIceAktar() {
    if (m_csvAktarici->isRunning()) {
        QMessageBox::warning(this, tr("Uyarı"), 
            tr("Bir CSV aktarımı zaten devam ediyor."));
        return;
    }
    
    QString dosyaYolu = QFileDialog::getOpenFileName(this,
        tr("CSV Dosyası Seç"), "", tr("CSV Dosyaları (*.csv)"));
    
    if (dosyaYolu.isEmpty()) return;
    
    // İlerleme dialogu
    m_ilerlemeDiyalogu = new QProgressDialog(tr("CSV dosyası içe aktarılıyor..."), 
        tr("İptal"), 0, 100, this);
    m_ilerlemeDiyalogu->setWindowModality(Qt::WindowModal);
    m_ilerlemeDiyalogu->setMinimumDuration(0);
    
    connect(m_ilerlemeDiyalogu, &QProgressDialog::canceled,
            m_csvAktarici, &CsvAktarici::iptalEt);
    
    m_csvAktarici->csvdenAktar(dosyaYolu);
}

void MainWindow::csvDisaAktar() {
    if (m_csvAktarici->isRunning()) {
        QMessageBox::warning(this, tr("Uyarı"), 
            tr("Bir CSV aktarımı zaten devam ediyor."));
        return;
    }
    
    QString dosyaYolu = QFileDialog::getSaveFileName(this,
        tr("CSV Dosyası Kaydet"), "hastalar.csv", tr("CSV Dosyaları (*.csv)"));
    
    if (dosyaYolu.isEmpty()) return;
    
    // İlerleme dialogu
    m_ilerlemeDiyalogu = new QProgressDialog(tr("CSV dosyası dışa aktarılıyor..."), 
        tr("İptal"), 0, 100, this);
    m_ilerlemeDiyalogu->setWindowModality(Qt::WindowModal);
    m_ilerlemeDiyalogu->setMinimumDuration(0);
    
    connect(m_ilerlemeDiyalogu, &QProgressDialog::canceled,
            m_csvAktarici, &CsvAktarici::iptalEt);
    
    m_csvAktarici->csveAktar(dosyaYolu);
}

void MainWindow::csvIlerlemeDegisti(int yuzde, const QString& mesaj) {
    if (m_ilerlemeDiyalogu) {
        m_ilerlemeDiyalogu->setValue(yuzde);
        m_ilerlemeDiyalogu->setLabelText(mesaj);
    }
}

void MainWindow::csvAktarimTamamlandi(int basarili, int hatali) {
    if (m_ilerlemeDiyalogu) {
        m_ilerlemeDiyalogu->close();
        delete m_ilerlemeDiyalogu;
        m_ilerlemeDiyalogu = nullptr;
    }
    
    m_hastaModel->yenile();
    durumCubuguGuncelle();
    
    QMessageBox::information(this, tr("CSV Aktarımı Tamamlandı"),
        tr("Başarılı: %1\nHatalı: %2").arg(basarili).arg(hatali));
}

// ============================================================================
// FAZ 4: AĞ PROGRAMLAMA FONKSİYONLARI
// ============================================================================

void MainWindow::sunucuBaslat() {
    if (m_sunucu->calisiyor()) {
        QMessageBox::information(this, tr("Bilgi"), 
            tr("Sunucu zaten çalışıyor."));
        return;
    }
    
    bool ok;
    int port = QInputDialog::getInt(this, tr("Sunucu Portu"),
        tr("Sunucu portunu girin:"), 12345, 1024, 65535, 1, &ok);
    
    if (ok) {
        if (m_sunucu->basla(port)) {
            statusBar()->showMessage(tr("Sunucu başlatıldı (Port: %1)").arg(port), 5000);
            QMessageBox::information(this, tr("Başarılı"), 
                tr("Sunucu %1 portunda başlatıldı.").arg(port));
        } else {
            QMessageBox::critical(this, tr("Hata"), 
                tr("Sunucu başlatılamadı!"));
        }
    }
}

void MainWindow::sunucuDurdur() {
    if (!m_sunucu->calisiyor()) {
        QMessageBox::information(this, tr("Bilgi"), 
            tr("Sunucu zaten durmuş durumda."));
        return;
    }
    
    m_sunucu->durdur();
    statusBar()->showMessage(tr("Sunucu durduruldu"), 3000);
    QMessageBox::information(this, tr("Başarılı"), 
        tr("Sunucu durduruldu."));
}
