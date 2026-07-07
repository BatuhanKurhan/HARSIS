# HARSIS - Hastane Randevu Sistemi

## Genel Bakış

HARSIS (Hastane Randevu Sistemi), hastanelerde hasta-doktor randevu yönetimini kolaylaştırmak için geliştirilmiş modern bir masaüstü uygulamasıdır. Uygulama, hasta kayıtları, doktor bilgileri ve randevu işlemlerini merkezi olarak yönetmeyi sağlar.

## Temel Özellikler

### 1. Hasta Yönetimi
- Hasta kayıt ekleme, görüntüleme ve arama
- TC kimlik numarası ile kimlik doğrulama
- Hasta bilgilerini güncelleme (isim, soyisim, doğum tarihi, telefon)
- Filtreleme ve sıralama özellikleri

### 2. Doktor Yönetimi
- Doktor profili oluşturma (sicil no, isim, soyisim, branş)
- Doktor çalışma saatlerini belirleme
- Branş bazlı filtreleme ve arama
- Doktor müsaitlik durumunu yönetme

### 3. Randevu Sistemi
- Hasta-doktor eşleştirmesi ile randevu oluşturma
- Tarih ve saat seçimi ile randevu planlama
- Randevu durumu takibi (Beklemede, Onaylandı, İptal, Tamamlandı)
- Randevu notları ekleme
- Gelişmiş arama ve filtreleme

### 4. Otomatik Hatırlatma Servisi
- Çok kanallı programlama ile arka planda çalışan hatırlatma sistemi
- Randevu zamanı yaklaşınca otomatik bildirim

### 5. Raporlama
- Randevu istatistikleri ve raporlar
- Doktor ve hasta bazlı analiz
- Çok iş parçacıklı rapor üretimi

### 6. Veri İçe/Dışa Aktarma
- CSV formatında veri içe aktarma
- CSV formatında veri dışa aktarma
- İlerleme göstergeli veri aktarımı

### 7. Ağ Programlama
- Sunucu-istemci mimarisi
- Uzaktan erişim desteği
- Network üzerinden veri senkronizasyonu

## Kullanılan Teknolojiler

### Programlama Dili ve Standart
- **C++20**: Modern C++ özellikleri (std::optional, concepts, ranges)
- **STL (Standard Template Library)**: Veri yapıları ve algoritmalar

### GUI Framework
- **Qt6**: Cross-platform GUI geliştirme
  - **Qt6::Widgets**: Masaüstü arayüz bileşenleri
  - **Qt6::Network**: Ağ programlama ve sunucu-istemci iletişimi
  - **Qt6::Charts**: Veri görselleştirme ve grafikler (opsiyonel)
  - **Qt6::PrintSupport**: Yazdırma özellikleri (opsiyonel)

### Tasarım Desenleri ve Mimariler
- **Model-View-Controller (MVC)**: Veri ve görünüm ayrımı
- **Generic Programming**: Template tabanlı Depo sınıfı
- **Thread-based Programming**: Çok kanallı programlama
- **Client-Server Architecture**: Ağ tabanlı mimari

### Özel Bileşenler
- **QTableView & QSortFilterProxyModel**: Dinamik tablo görünümleri
- **Custom Models**: HastaModel, DoktorModel, RandevuModel
- **Thread Workers**: HatirlatmaServisi, RaporUretici, CsvAktarici
- **QTcpServer/QTcpSocket**: Network iletişimi

### Geliştirme Araçları
- **CMake**: Derleme sistemi yönetimi (v3.20+)
- **Qt MOC (Meta-Object Compiler)**: Qt sinyalleri ve slotları için otomatik kod üretimi
- **qmake**: Alternatif Qt derleme sistemi

## Sistem Gereksinimleri

- Modern C++ derleyici (C++20 desteği)
- Qt6 Framework
- CMake 3.20 veya üzeri
- macOS, Linux veya Windows işletim sistemi

## Derleme

```bash
# Qt6 kurulumu (macOS)
brew install qt6

# CMake ile derleme
mkdir build
cd build
cmake ..
make

# Çalıştırma
./harsis_qt      # GUI versiyonu
./harsis         # Konsol versiyonu
```

## Proje Yapısı

- **include/**: Header dosyaları
- **src/**: Kaynak kod dosyaları
- **CMakeLists.txt**: CMake derleme yapılandırması
- **harsis_qt.pro**: qmake proje dosyası

## Geliştirme Fazları

1. **Faz 1**: STL tabanlı konsol uygulaması
2. **Faz 2**: Qt6 GUI ve temel işlevsellik
3. **Faz 3**: Çok kanallı programlama (threading)
4. **Faz 4**: Ağ programlama (networking)
