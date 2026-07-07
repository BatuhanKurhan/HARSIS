#pragma once

/**
 * main_window.h -- Ana Pencere
 *
 * QMainWindow tabanlı ana uygulama penceresi.
 * Menü, araç çubuğu, sekmeli görünüm ve durum çubuğu içerir.
 */

#include "depo.h"
#include "varliklar.h"
#include "hasta_model.h"
#include "doktor_model.h"
#include "randevu_model.h"
#include "hatirlatma_servisi.h"
#include "rapor_uretici.h"
#include "csv_aktarici.h"
#include "sunucu.h"

#include <QMainWindow>
#include <QTabWidget>
#include <QTableView>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressDialog>
#include <memory>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // Menü aksiyonları
    void dosyaKaydet();
    void dosyaYukle();
    void hastaEkle();
    void doktorEkle();
    void randevuAl();
    void hakkinda();
    
    // Arama ve filtreleme
    void hastaAra(const QString& metin);
    void doktorAra(const QString& metin);
    void randevuAra(const QString& metin);
    void bransFiltrele(const QString& brans);
    
    // Faz 3: Thread işlemleri
    void hatirlatmaServisiniBaslat();
    void hatirlatmaServisiniDurdur();
    void raporUret();
    void csvIceAktar();
    void csvDisaAktar();
    
    // Faz 4: Ağ programlama
    void sunucuBaslat();
    void sunucuDurdur();
    
    // Thread sinyalleri
    void hatirlatmaGeldi(const QString& mesaj);
    void raporHazir(const RaporSonucu& sonuc);
    void csvIlerlemeDegisti(int yuzde, const QString& mesaj);
    void csvAktarimTamamlandi(int basarili, int hatali);

private:
    void menuOlustur();
    void aracCubuguOlustur();
    void sekmeliGorunumOlustur();
    void durumCubuguGuncelle();
    void ayarlariKaydet();
    void ayarlariYukle();
    
    // Veri depoları
    std::unique_ptr<Depo<std::string, Hasta>> m_hastalar;
    std::unique_ptr<Depo<std::string, Doktor>> m_doktorlar;
    std::unique_ptr<Depo<int, Randevu>> m_randevular;
    int m_sonRandevuId;
    
    // Model/View bileşenleri
    QTabWidget* m_tabWidget;
    
    // Hasta sekmesi
    QTableView* m_hastaTablosu;
    HastaModel* m_hastaModel;
    QSortFilterProxyModel* m_hastaProxyModel;
    QLineEdit* m_hastaAramaKutusu;
    
    // Doktor sekmesi
    QTableView* m_doktorTablosu;
    DoktorModel* m_doktorModel;
    QSortFilterProxyModel* m_doktorProxyModel;
    QLineEdit* m_doktorAramaKutusu;
    QComboBox* m_bransComboBox;
    
    // Randevu sekmesi
    QTableView* m_randevuTablosu;
    RandevuModel* m_randevuModel;
    QSortFilterProxyModel* m_randevuProxyModel;
    QLineEdit* m_randevuAramaKutusu;
    
    // Menü ve araç çubuğu
    QMenu* m_dosyaMenu;
    QMenu* m_duzenMenu;
    QMenu* m_yardimMenu;
    QMenu* m_servislerMenu;
    QMenu* m_agMenu;
    QToolBar* m_aracCubugu;
    
    // Faz 3: Thread servisleri
    HatirlatmaServisi* m_hatirlatmaServisi;
    RaporUretici* m_raporUretici;
    CsvAktarici* m_csvAktarici;
    QProgressDialog* m_ilerlemeDiyalogu;
    
    // Faz 4: Ağ servisleri
    Sunucu* m_sunucu;
    
    void ornekVerilerYukle();
};
