/**
 * main.cpp -- HARSIS Başlangıç Noktası
 *
 * Hastane Randevu Sistemi - Faz 1 başlangıç kodu.
 * Bu dosya derlenip çalıştırılabilir durumdadır.
 * GOREV işaretli yerleri tamamlayarak sistemi geliştirin.
 *
 * Derleme: g++ -std=c++20 -o harsis src/main.cpp
 * Çalıştırma: ./harsis
 */

#include "../include/depo.h"
#include "../include/varliklar.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <ranges>
#include <string>
#include <unordered_map>

// === Dosya Islemleri Fonksiyonlari ===

// Hasta deposunu dosyaya kaydet
void hasta_dosyaya_kaydet(const std::string &dosya_yolu, 
                          const Depo<std::string, Hasta> &depo) {
  std::ofstream dosya(dosya_yolu, std::ios::binary);
  if (!dosya) {
    std::cerr << "Dosya acilamadi: " << dosya_yolu << "\n";
    return;
  }
  
  auto veriler = depo.tumunu_al();
  size_t boyut = veriler.size();
  dosya.write(reinterpret_cast<const char*>(&boyut), sizeof(boyut));
  
  for (const auto &[tc, hasta] : veriler) {
    size_t tc_uzunluk = hasta.tc_no.size();
    dosya.write(reinterpret_cast<const char*>(&tc_uzunluk), sizeof(tc_uzunluk));
    dosya.write(hasta.tc_no.data(), tc_uzunluk);
    
    size_t isim_uzunluk = hasta.isim.size();
    dosya.write(reinterpret_cast<const char*>(&isim_uzunluk), sizeof(isim_uzunluk));
    dosya.write(hasta.isim.data(), isim_uzunluk);
    
    size_t soyisim_uzunluk = hasta.soyisim.size();
    dosya.write(reinterpret_cast<const char*>(&soyisim_uzunluk), sizeof(soyisim_uzunluk));
    dosya.write(hasta.soyisim.data(), soyisim_uzunluk);
    
    size_t dogum_uzunluk = hasta.dogum_tarihi.size();
    dosya.write(reinterpret_cast<const char*>(&dogum_uzunluk), sizeof(dogum_uzunluk));
    dosya.write(hasta.dogum_tarihi.data(), dogum_uzunluk);
    
    size_t telefon_uzunluk = hasta.telefon.size();
    dosya.write(reinterpret_cast<const char*>(&telefon_uzunluk), sizeof(telefon_uzunluk));
    dosya.write(hasta.telefon.data(), telefon_uzunluk);
  }
  
  std::cout << "  " << boyut << " hasta dosyaya kaydedildi: " << dosya_yolu << "\n";
}

// Hasta deposunu dosyadan oku
Depo<std::string, Hasta> hasta_dosyadan_oku(const std::string &dosya_yolu) {
  Depo<std::string, Hasta> depo;
  std::ifstream dosya(dosya_yolu, std::ios::binary);
  
  if (!dosya) {
    std::cerr << "Dosya acilamadi: " << dosya_yolu << "\n";
    return depo;
  }
  
  size_t boyut;
  dosya.read(reinterpret_cast<char*>(&boyut), sizeof(boyut));
  
  for (size_t i = 0; i < boyut; ++i) {
    Hasta hasta;
    
    size_t tc_uzunluk;
    dosya.read(reinterpret_cast<char*>(&tc_uzunluk), sizeof(tc_uzunluk));
    hasta.tc_no.resize(tc_uzunluk);
    dosya.read(hasta.tc_no.data(), tc_uzunluk);
    
    size_t isim_uzunluk;
    dosya.read(reinterpret_cast<char*>(&isim_uzunluk), sizeof(isim_uzunluk));
    hasta.isim.resize(isim_uzunluk);
    dosya.read(hasta.isim.data(), isim_uzunluk);
    
    size_t soyisim_uzunluk;
    dosya.read(reinterpret_cast<char*>(&soyisim_uzunluk), sizeof(soyisim_uzunluk));
    hasta.soyisim.resize(soyisim_uzunluk);
    dosya.read(hasta.soyisim.data(), soyisim_uzunluk);
    
    size_t dogum_uzunluk;
    dosya.read(reinterpret_cast<char*>(&dogum_uzunluk), sizeof(dogum_uzunluk));
    hasta.dogum_tarihi.resize(dogum_uzunluk);
    dosya.read(hasta.dogum_tarihi.data(), dogum_uzunluk);
    
    size_t telefon_uzunluk;
    dosya.read(reinterpret_cast<char*>(&telefon_uzunluk), sizeof(telefon_uzunluk));
    hasta.telefon.resize(telefon_uzunluk);
    dosya.read(hasta.telefon.data(), telefon_uzunluk);
    
    depo.ekle(hasta.tc_no, hasta);
  }
  
  std::cout << "  " << boyut << " hasta dosyadan okundu: " << dosya_yolu << "\n";
  return depo;
}

int main() {
  std::cout << "=== HARSIS - Hastane Randevu Sistemi ===\n\n";

  // -- Hasta deposu olustur ve ornek veriler ekle --
  Depo<std::string, Hasta> hastalar;

  hastalar.ekle("12345678901",
                {"12345678901", "Ali", "Yilmaz", "1985-03-15", "555-0101"});
  hastalar.ekle("23456789012",
                {"23456789012", "Zeynep", "Kara", "1990-07-22", "555-0102"});
  hastalar.ekle("34567890123",
                {"34567890123", "Mehmet", "Demir", "1978-11-05", "555-0103"});

  std::cout << "Hasta sayisi: " << hastalar.boyut() << "\n\n";

  // Tum hastalari listele
  std::cout << "--- Hasta Listesi ---\n";
  for (const auto &[tc, hasta] : hastalar.tumunu_al()) {
    std::cout << "  " << hasta << "\n";
  }
  std::cout << "\n";

  // -- Doktor deposu olustur --
  Depo<std::string, Doktor> doktorlar;

  doktorlar.ekle("D001", {"D001",
                          "Ayse",
                          "Ozturk",
                          "Dahiliye",
                          {"09:00", "09:30", "10:00", "10:30", "11:00"}});
  doktorlar.ekle("D002", {"D002",
                          "Hasan",
                          "Celik",
                          "Ortopedi",
                          {"13:00", "13:30", "14:00", "14:30"}});
  doktorlar.ekle("D003", {"D003",
                          "Fatma",
                          "Sahin",
                          "Dahiliye",
                          {"09:00", "09:30", "10:00", "14:00", "14:30"}});

  std::cout << "Doktor sayisi: " << doktorlar.boyut() << "\n\n";

  std::cout << "--- Doktor Listesi ---\n";
  for (const auto &[sicil, doktor] : doktorlar.tumunu_al()) {
    std::cout << "  " << doktor << "\n";
  }
  std::cout << "\n";

  // -- Randevu deposu olustur --
  Depo<int, Randevu> randevular;

  randevular.ekle(1, {1, "12345678901", "D001", "2026-04-15", "09:00",
                      RandevuDurum::Onaylandi, "Kontrol muayenesi"});
  randevular.ekle(2, {2, "23456789012", "D002", "2026-04-15", "13:30",
                      RandevuDurum::Beklemede, std::nullopt});
  randevular.ekle(3,
                  {3, "12345678901", "D003", "2026-04-16", "10:00",
                   RandevuDurum::Tamamlandi, "Kan tahlili sonuclari normal"});
  randevular.ekle(4, {4, "34567890123", "D001", "2026-04-15", "10:30",
                      RandevuDurum::Iptal, "Hasta tarafindan iptal edildi"});

  std::cout << "--- Randevu Listesi ---\n";
  for (const auto &[id, randevu] : randevular.tumunu_al()) {
    std::cout << "  " << randevu << "\n";
  }
  std::cout << "\n";

  // === GOREV 1.3a - Brans Filtreleme (10 dk) ===
  // TODO: "Dahiliye" bransindaki doktorlari filtrele ve listele.
  //       Ipucu: doktorlar.filtrele(...) metodunu kullan.
  // =============================================
  std::cout << "--- Dahiliye Bransi Doktorlari ---\n";
  auto dahiliye_doktorlari = doktorlar.filtrele([](const Doktor &d) {
    return d.brans == "Dahiliye";
  });
  for (const auto &doktor : dahiliye_doktorlari) {
    std::cout << "  " << doktor << "\n";
  }
  std::cout << "\n";

  // === GOREV 1.3b - Cakisma Kontrolu (15 dk) ===
  // TODO: Belirli bir doktorun belirli bir tarih ve saatte
  //       baska bir randevusu olup olmadigini kontrol et.
  //       Ipucu: randevular.filtrele(...) ile doktor ve
  //              tarih/saat eslesenleri bul.
  // =============================================
  std::cout << "--- Cakisma Kontrolu ---\n";
  std::string kontrol_doktor = "D001";
  std::string kontrol_tarih = "2026-04-15";
  std::string kontrol_saat = "09:00";
  
  auto cakisan_randevular = randevular.filtrele([&](const Randevu &r) {
    return r.sicil_no == kontrol_doktor && 
           r.tarih == kontrol_tarih && 
           r.saat == kontrol_saat &&
           r.durum != RandevuDurum::Iptal;
  });
  
  if (!cakisan_randevular.empty()) {
    std::cout << "  UYARI: " << kontrol_doktor << " doktorunun " 
              << kontrol_tarih << " " << kontrol_saat 
              << " saatinde randevusu var!\n";
    for (const auto &r : cakisan_randevular) {
      std::cout << "    " << r << "\n";
    }
  } else {
    std::cout << "  " << kontrol_doktor << " doktoru " 
              << kontrol_tarih << " " << kontrol_saat 
              << " saatinde musait.\n";
  }
  std::cout << "\n";

  // === GOREV 1.3c - Tarih Bazli Siralama (10 dk) ===
  // TODO: Belirli bir tarihteki tum randevulari saate gore sirala.
  //       Ipucu: tumunu_al() ile alip bir vector'e kopyala,
  //              sonra std::ranges::sort kullan.
  // =================================================
  std::cout << "--- 2026-04-15 Tarihindeki Randevular (Saate Gore Sirali) ---\n";
  std::string hedef_tarih = "2026-04-15";
  
  auto gunluk_randevular = randevular.filtrele([&](const Randevu &r) {
    return r.tarih == hedef_tarih;
  });
  
  // Saate gore sirala
  std::ranges::sort(gunluk_randevular, [](const Randevu &a, const Randevu &b) {
    return a.saat < b.saat;
  });
  
  for (const auto &r : gunluk_randevular) {
    std::cout << "  " << r << "\n";
  }
  std::cout << "\n";

  // === GOREV 1.3d - Acil Hasta Onceliklendirme (15 dk) ===
  // TODO: std::priority_queue kullanarak acil hastalari
  //       oncelik sirasina gore isle.
  // =======================================================
  std::cout << "--- Acil Hasta Onceliklendirme ---\n";
  
  // Acil hasta yapisi (oncelik: 1=en acil, 5=normal)
  struct AcilHasta {
    std::string tc_no;
    std::string isim;
    int oncelik; // 1-5 arasi, 1 en acil
    
    bool operator<(const AcilHasta &diger) const {
      return oncelik > diger.oncelik; // Kucuk sayi daha yuksek oncelik
    }
  };
  
  std::priority_queue<AcilHasta> acil_kuyruk;
  
  // Ornek acil hastalar ekle
  acil_kuyruk.push({"12345678901", "Ali Yilmaz", 3});
  acil_kuyruk.push({"23456789012", "Zeynep Kara", 1}); // En acil
  acil_kuyruk.push({"34567890123", "Mehmet Demir", 2});
  acil_kuyruk.push({"45678901234", "Ayse Yildiz", 5}); // Normal
  
  std::cout << "  Acil hastalar oncelik sirasina gore:\n";
  int sira = 1;
  while (!acil_kuyruk.empty()) {
    auto hasta = acil_kuyruk.top();
    acil_kuyruk.pop();
    std::cout << "    " << sira++ << ". " << hasta.isim 
              << " (TC: " << hasta.tc_no 
              << ", Oncelik: " << hasta.oncelik << ")\n";
  }
  std::cout << "\n";

  // === GOREV 1.4 - Dosya Islemleri (20 dk) ===
  // TODO: Hasta ve randevu depolarini bir dosyaya kaydet ve geri oku.
  //       Ikili (binary) format kullanabilirsin.
  // ============================================
  std::cout << "--- Dosya Islemleri ---\n";
  
  // Hasta deposunu kaydet
  hasta_dosyaya_kaydet("hastalar.dat", hastalar);
  
  // Dosyadan geri oku
  auto yuklenen_hastalar = hasta_dosyadan_oku("hastalar.dat");
  
  std::cout << "  Yuklenen hasta sayisi: " << yuklenen_hastalar.boyut() << "\n";
  
  // TC no ile hizli arama (unordered_map kullanimi)
  std::cout << "\n--- TC No ile Hizli Hasta Arama ---\n";
  std::unordered_map<std::string, Hasta> hasta_map;
  for (const auto &[tc, hasta] : hastalar.tumunu_al()) {
    hasta_map[tc] = hasta;
  }
  
  std::string aranan_tc = "23456789012";
  auto bulunan = hasta_map.find(aranan_tc);
  if (bulunan != hasta_map.end()) {
    std::cout << "  TC " << aranan_tc << " bulundu: " << bulunan->second << "\n";
  } else {
    std::cout << "  TC " << aranan_tc << " bulunamadi.\n";
  }
  std::cout << "\n";

  std::cout << "=== Program sona erdi ===\n";
  return 0;
}

/*
BEKLENEN CIKTI:
---------------
=== HARSIS - Hastane Randevu Sistemi ===

Hasta sayisi: 3

--- Hasta Listesi ---
  [TC: 12345678901] Ali Yilmaz (Dogum: 1985-03-15, Tel: 555-0101)
  [TC: 23456789012] Zeynep Kara (Dogum: 1990-07-22, Tel: 555-0102)
  [TC: 34567890123] Mehmet Demir (Dogum: 1978-11-05, Tel: 555-0103)

Doktor sayisi: 3

--- Doktor Listesi ---
  [Sicil: D001] Dr. Ayse Ozturk (Brans: Dahiliye, Uygun saatler: 09:00 09:30
10:00 10:30 11:00) [Sicil: D002] Dr. Hasan Celik (Brans: Ortopedi, Uygun
saatler: 13:00 13:30 14:00 14:30) [Sicil: D003] Dr. Fatma Sahin (Brans:
Dahiliye, Uygun saatler: 09:00 09:30 10:00 14:00 14:30)

--- Randevu Listesi ---
  [Randevu #1] Hasta TC: 12345678901, Doktor: D001, Tarih: 2025-04-15 09:00,
Durum: Onaylandi, Not: Kontrol muayenesi [Randevu #2] Hasta TC: 23456789012,
Doktor: D002, Tarih: 2025-04-15 13:30, Durum: Beklemede, Not: --- [Randevu #3]
Hasta TC: 12345678901, Doktor: D003, Tarih: 2025-04-16 10:00, Durum: Tamamlandi,
Not: Kan tahlili sonuclari normal [Randevu #4] Hasta TC: 34567890123, Doktor:
D001, Tarih: 2025-04-15 10:30, Durum: Iptal, Not: Hasta tarafindan iptal edildi

=== Program sona erdi ===
*/