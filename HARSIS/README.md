# HARSIS Kullanım Kılavuzu

## Hasta İşlemleri

### Yeni Hasta Ekleme

1. **Menü → Düzen → Hasta Ekle** veya **Araç Çubuğu → Hasta Ekle**
2. Açılan formda bilgileri doldurun:
   - **TC Kimlik No**: 11 haneli TC kimlik numarası (zorunlu)
   - **İsim**: Hastanın adı (zorunlu)
   - **Soyisim**: Hastanın soyadı (zorunlu)
   - **Doğum Tarihi**: Takvimden seçin (varsayılan: 1990-01-01)
   - **Telefon**: İletişim numarası (örn: 555-0123)
3. **Tamam** butonuna tıklayın

#### Önemli Notlar
- TC kimlik numarası mutlaka 11 haneli olmalıdır
- Aynı TC numarasıyla ikinci kez hasta eklenemez
- Tüm alanlar doldurulmalıdır

### Hasta Listesini Görüntüleme

1. **Hastalar** sekmesine tıklayın
2. Tablo sütunlarına tıklayarak sıralama yapabilirsiniz:
   - TC No
   - İsim
   - Soyisim
   - Doğum Tarihi
   - Telefon

### Hasta Arama

1. **Hastalar** sekmesinde üstteki arama kutusunu kullanın
2. TC no, isim veya soyisim yazın
3. Sonuçlar anlık olarak filtrelenir

---

## Doktor İşlemleri

### Yeni Doktor Ekleme

1. **Menü → Düzen → Doktor Ekle** veya **Araç Çubuğu → Doktor Ekle**
2. Açılan formda bilgileri doldurun:
   - **Sicil No**: Doktorun sicil numarası (örn: D001)
   - **İsim**: Doktorun adı
   - **Soyisim**: Doktorun soyadı
   - **Branş**: Açılır listeden seçin (Dahiliye, Ortopedi, vb.)
3. **Uygun Saatler** bölümünde:
   - Açılır listeden saat seçin
   - **Ekle** butonuna tıklayın
   - İstediğiniz kadar saat ekleyebilirsiniz
   - Silmek için saati seçip **Sil** butonuna tıklayın
4. **Tamam** butonuna tıklayın


### Doktor Listesini Görüntüleme

1. **Doktorlar** sekmesine tıklayın
2. Tablo sütunlarına tıklayarak sıralama yapabilirsiniz:
   - Sicil No
   - İsim
   - Soyisim
   - Branş

### Doktor Arama ve Filtreleme

#### Branş Filtreleme
1. **Doktorlar** sekmesinde üstteki **Branş** açılır listesini kullanın
2. Bir branş seçin (örn: Dahiliye)
3. Sadece o branştaki doktorlar gösterilir
4. **Tümü** seçeneği ile tüm doktorları görebilirsiniz

#### Metin Arama
1. Arama kutusuna sicil no, isim veya soyisim yazın
2. Sonuçlar anlık olarak filtrelenir

---

## Randevu İşlemleri

### Yeni Randevu Oluşturma

1. **Menü → Düzen → Randevu Al** veya **Araç Çubuğu → Randevu Al**
2. Açılan formda sırasıyla:
   
   **a) Hasta Seçimi**
   - Açılır listeden hastayı seçin
   - Format: "TC No - İsim Soyisim"
   
   **b) Branş Seçimi**
   - Açılır listeden branş seçin
   - Doktor listesi otomatik olarak bu branşa göre filtrelenir
   
   **c) Doktor Seçimi**
   - Filtrelenmiş doktor listesinden seçim yapın
   - Format: "Dr. İsim Soyisim (Sicil No)"
   - Uygun saatler otomatik olarak yüklenir
   
   **d) Tarih Seçimi**
   - Takvim simgesine tıklayın
   - Bugünden önceki tarihler seçilemez
   - Format: YYYY-MM-DD
   
   **e) Saat Seçimi**
   - Doktorun uygun saatlerinden birini seçin

3. **Randevu Oluştur** butonuna tıklayın


