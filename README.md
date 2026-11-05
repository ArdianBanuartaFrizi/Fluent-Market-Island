# Fluent Market Island

**Mod Windhawk untuk Windows** yang menampilkan harga aset pasar dalam
bentuk capsule/Dynamic Island yang dapat diperluas menjadi panel market.

- **Maintainer:** Banuarta
- **Mod ID:** `market-island-dock`
- **Versi repositori:** `2.9.2`
- **Bahasa kode dan antarmuka:** C++ / Windows API
- **Platform:** Windows 10/11 dengan Windhawk (Windows 11
  direkomendasikan)

> **Catatan pengembangan:** repositori ini menggunakan kode sumber mod
> yang diberikan oleh pemilik proyek dan telah direvisi dengan bantuan
> AI. Nama `Banuarta` digunakan sebagai maintainer proyek, bukan sebagai
> klaim bahwa setiap baris kode awal ditulis tanpa bantuan pihak lain.

## 1. Gambaran proyek

Fluent Market Island adalah widget desktop berbasis Windhawk. Tampilan
ringkas berupa capsule menampilkan satu instrumen terpilih. Ketika
dibuka, capsule berubah menjadi panel yang berisi watchlist, kategori
aset, pencarian instrumen, dan tombol refresh.

### Fitur utama

- Capsule yang dapat diperluas dan ditutup kembali.
- Enam posisi: kiri, tengah, dan kanan pada sisi atas maupun bawah
  layar.
- Watchlist bawaan:
  - **Crypto:** BTC, ETH, SOL, XRP, BNB.
  - **US Stocks / ETF:** SPY, QQQ, DIA, NVDA, AAPL.
  - **Indonesia / IDX:** BBCA, BBRI, BMRI, TLKM, ASII.
- Pencarian aset tambahan melalui kolom pencarian.
- Tema: Fluent, Nusa, Purple, Ocean, Aurora, dan Light.
- Material: Acrylic, Glass, dan None.
- Pengaturan ukuran, font, warna, refresh data, dan durasi animasi.
- **Shape Background Opacity** yang terpisah dari **Fluent Glass Tint**.

## 2. Pratinjau pengaturan transparansi

Dua pengaturan ini mempunyai fungsi berbeda:

| Pengaturan        | Rentang | Fungsi                                               |
|:------------------|--------:|:-----------------------------------------------------|
| `ShapeOpacity`    |  0–100% | Mengatur opacity keseluruhan permukaan capsule/panel |
| `FluentGlassTint` |   0–255 | Mengatur kekuatan tint warna pada material glass     |

Contoh `ShapeOpacity`:

| Nilai | Tampilan                            |
|------:|:------------------------------------|
|  100% | Background terlihat penuh           |
|   75% | Background lebih transparan         |
|   50% | Background setengah transparan      |
|   25% | Background sangat transparan        |
|    0% | Permukaan background tidak terlihat |

Teks dan kontrol interaktif tidak ikut dibuat transparan secara global
agar tetap terbaca. Pengaturan ini mengendalikan permukaan shape, bukan
seluruh isi widget.

## 3. Struktur repositori

``` text
Fluent-Market-Island-GitHub/
├── README.md
├── CHANGELOG.md
├── REFERENCES.md
├── LICENSE-STATUS.md
├── .gitignore
├── src/
│   └── market-island-dock.wh.cpp
└── docs/
    └── ARCHITECTURE.md
```

File utama mod adalah `src/market-island-dock.wh.cpp`. Windhawk membaca
metadata, kode C++, dan blok pengaturan yang berada di dalam satu file
tersebut.

## 4. Yang perlu diinstal

### Wajib

1.  **Windows 10 atau Windows 11.** Windows 11 direkomendasikan untuk
    tampilan glass yang paling sesuai.
2.  **Windhawk** dari situs resmi: <https://windhawk.net/>.
3.  **Koneksi internet** jika ingin mengambil data harga dan melakukan
    pencarian instrumen.

### Tidak perlu diinstal secara manual untuk pemasangan melalui Windhawk

- Visual Studio.
- Compiler C++ terpisah.
- Python.
- Node.js.
- Paket NuGet.
- API key Yahoo Finance.

Windhawk menyediakan alur kompilasi mod melalui editor bawaannya. Kode
menggunakan API Windows yang tersedia di sistem, termasuk WinHTTP, GDI+,
Win32, dan Windows Composition. Kompatibilitas fitur backdrop tetap
dapat berbeda menurut versi/build Windows.

## 5. Cara memasang mod di Windhawk

### Instalasi pertama

1.  Unduh dan instal Windhawk dari <https://windhawk.net/>.
2.  Jalankan Windhawk.
3.  Buka editor mod atau fitur untuk membuat mod baru yang tersedia pada
    versi Windhawk yang digunakan.
4.  Buka file `src/market-island-dock.wh.cpp` dari repositori ini.
5.  Salin seluruh isi file.
6.  Tempelkan ke editor kode mod Windhawk.
7.  Simpan perubahan dan jalankan **Compile**.
8.  Jika kompilasi berhasil, aktifkan mod.
9.  Buka pengaturan mod untuk mengatur posisi, tema, ukuran, dan
    transparansi.

### Memperbarui instalasi yang sudah ada

1.  Buka mod **Fluent Market Island** di Windhawk.
2.  Pilih **Edit Code**.
3.  Ganti seluruh kode lama dengan isi file `.wh.cpp` dari repositori
    ini.
4.  Simpan dan compile ulang.
5.  Periksa pengaturan mod.

Mod ID `market-island-dock` sengaja dipertahankan agar identitas mod dan
pengaturan yang sudah tersimpan tidak berubah karena pergantian ID.

> Jika kompilasi gagal, simpan pesan error lengkap dari Windhawk. Jangan
> menganggap file sudah teruji compile hanya karena struktur repositori
> berhasil dibuat; kompilasi akhir harus dilakukan pada Windhawk di
> Windows.

## 6. Cara menggunakan widget

1.  Klik capsule untuk membuka panel.
2.  Pilih kategori **CRYPTO**, **US / ETF**, atau **INDONESIA**.
3.  Pilih instrumen pada daftar untuk menampilkannya di capsule.
4.  Untuk mencari instrumen lain, klik kolom pencarian, ketik nama atau
    simbol, lalu tekan **Enter**.
5.  Pilih salah satu hasil pencarian untuk mengambil kuotasi instrumen
    tersebut.
6.  Gunakan tombol refresh untuk meminta pembaruan data.
7.  Klik capsule kembali untuk menutup panel.

Hasil pencarian dan harga bergantung pada respons endpoint data
eksternal. Jika data tidak tersedia, widget dapat menampilkan `--`.

## 7. Pengaturan Windhawk

| Nama pengaturan      |   Default | Keterangan                                       |
|:---------------------|----------:|:-------------------------------------------------|
| `RefreshSeconds`     |       300 | Interval pembaruan data, 60–3600 detik           |
| `OffsetX`            |         0 | Offset horizontal                                |
| `OffsetY`            |        30 | Offset vertikal                                  |
| `FontSize`           |        16 | Ukuran font harga                                |
| `FluentGlassTint`    |       116 | Kekuatan warna tint glass, 0–255                 |
| `ShapeOpacity`       |       100 | Opacity background shape, 0–100%                 |
| `ScalePercent`       |       100 | Skala antarmuka, 70–150%                         |
| `PillWidth`          |       380 | Lebar capsule                                    |
| `PillHeight`         |        44 | Tinggi capsule                                   |
| `AnimationDuration`  |       420 | Durasi animasi, 100–800 ms                       |
| `AlwaysOnTop`        |      true | Menjaga widget di atas jendela lain              |
| `HideWhenFullscreen` |      true | Menyembunyikan widget ketika aplikasi fullscreen |
| `Position`           | TopCenter | Enam posisi anchor                               |
| `GlassTheme`         |    Fluent | Fluent, Nusa, Purple, Ocean, Aurora, Light       |
| `BlurStyle`          |   Acrylic | Acrylic, Glass, None                             |
| `FontFace`           |  Segoe UI | Keluarga font                                    |
| `TextColor`          |  \#F5F7FA | Warna teks utama                                 |
| `UpColor`            |  \#31D98B | Warna perubahan positif                          |
| `DownColor`          |  \#FF5B67 | Warna perubahan negatif                          |
| `FlatColor`          |  \#A0AAB9 | Warna perubahan netral                           |
| `MutedColor`         |  \#A0AAB9 | Warna teks sekunder                              |

## 8. Cara kerja teknis

### 8.1 Siklus Windhawk

Mod ditargetkan ke `windhawk.exe` dan menyertakan pola launcher tool-mod
untuk menjalankan logika widget dalam proses khusus. Kode
menginisialisasi pengaturan, membuat jendela widget, mengelola thread
data, dan menangani perubahan konfigurasi.

### 8.2 Pemisahan layer

Widget memakai dua jalur visual:

- **Layer antarmuka:** GDI+ menggambar teks, angka, label, tombol, dan
  elemen panel pada permukaan transparan.
- **Layer glass:** Windows Composition membuat visual backdrop, tint,
  sheen, dan rim. Geometri rounded rectangle digunakan untuk membatasi
  area glass.

Pemisahan ini memungkinkan background glass diatur tanpa membuat teks
dan kontrol ikut pudar.

### 8.3 Perhitungan opacity

Nilai `ShapeOpacity` dikonversi dari persentase menjadi faktor 0,0–1,0.

- Pada jalur Composition, faktor diterapkan pada opacity container
  shape.
- Pada jalur fallback GDI+, faktor diterapkan pada alpha warna
  background dan border.
- Pada jalur fallback accent window, alpha jendela glass disesuaikan
  dengan nilai yang sama.
- Nilai 0 menghilangkan permukaan shape, sedangkan teks tetap digambar
  pada layer antarmuka.

`FluentGlassTint` tetap mengendalikan alpha tint warna dan tidak
digabungkan dengan nilai persentase tersebut.

### 8.4 Animasi

Animasi capsule ke panel menggunakan progres transisi yang dihitung
berdasarkan waktu. Interval frame berupaya mengikuti refresh compositor
melalui informasi timing DWM, dengan timer fallback ketika informasi
tersebut tidak tersedia.

### 8.5 Pengambilan data

WinHTTP membuat permintaan HTTPS ke host `query1.finance.yahoo.com`.
Data JSON dibaca dan diparsing untuk mengambil harga serta harga
penutupan sebelumnya. Persentase perubahan dihitung dengan:

``` text
((harga_sekarang / harga_sebelumnya) - 1) × 100
```

Pembaruan watchlist berjalan di thread terpisah agar permintaan jaringan
tidak dilakukan langsung di jalur rendering antarmuka.

## 9. Koneksi internet, privasi, dan batasan data

Kode saat ini mengakses endpoint berikut:

- Kuotasi:
  `https://query1.finance.yahoo.com/v8/finance/chart/{symbol}?range=1d&interval=1m`
- Pencarian:
  `https://query1.finance.yahoo.com/v1/finance/search?q={query}&quotesCount=8&newsCount=0`

Simbol instrumen dan kata kunci pencarian dikirim ke layanan tersebut
melalui HTTPS. Kode ini tidak meminta API key Yahoo.

**Penting:** endpoint tersebut bukan API publik resmi yang
terdokumentasi dan dijamin stabil. Yahoo dapat mengubah format respons,
membatasi permintaan, atau menghentikan akses.

Ketentuan Yahoo membatasi pengumpulan data otomatis tanpa izin
sebelumnya dan membatasi penggunaan ulang data untuk membuat widget atau
sumber data agregat. Karena itu:

- Jangan menganggap pengambilan data otomatis ini otomatis diizinkan.
- Periksa ketentuan yang berlaku untuk wilayah dan penggunaan kamu.
- Dapatkan izin yang diperlukan atau ganti penyedia data dengan API yang
  memiliki lisensi dan ketentuan penggunaan yang sesuai sebelum
  mendistribusikan layanan market-data kepada publik atau menggunakannya
  secara komersial.
- Jangan mengemas ulang atau mendistribusikan data Yahoo tanpa hak yang
  sesuai.

Data harga dapat terlambat, tidak lengkap, atau gagal diambil. Widget
ini bukan platform eksekusi transaksi dan bukan sumber tunggal untuk
keputusan investasi.

## 10. Pemecahan masalah

| Masalah                           | Hal yang perlu diperiksa                                                               |
|:----------------------------------|:---------------------------------------------------------------------------------------|
| Harga tampil `--`                 | Periksa koneksi internet, simbol, respons Yahoo, dan kemungkinan pembatasan permintaan |
| Pencarian tidak menghasilkan data | Coba simbol/nama yang lebih spesifik atau periksa endpoint pencarian                   |
| Glass tidak terlihat              | Coba material lain, periksa build Windows, atau pilih `BlurStyle: None`                |
| Slider opacity tidak terlihat     | Pastikan file yang dipasang sudah berisi pengaturan `ShapeOpacity` dan compile ulang   |
| Background masih terlihat saat 0% | Pastikan menggunakan versi 2.9.2 dan periksa jalur fallback yang aktif                 |
| Compile gagal                     | Periksa error lengkap dan versi Windhawk/compiler yang digunakan                       |
| Animasi terasa patah              | Periksa beban sistem, refresh rate, dan log Windhawk                                   |

## 11. Mengunggah ke GitHub

1.  Masuk ke GitHub.
2.  Buat repository baru, misalnya `fluent-market-island`.
3.  Pilih visibilitas Public atau Private.
4.  Unggah isi folder repositori ini, bukan hanya file ZIP.
5.  Pastikan `README.md`, `src/`, `docs/`, `CHANGELOG.md`,
    `REFERENCES.md`, dan `.gitignore` ikut terunggah.
6.  Buat commit pertama, misalnya `Initial public release - v2.9.2`.

### Alternatif menggunakan Git

``` bash
git init
git add .
git commit -m "Initial release v2.9.2"
git branch -M main
git remote add origin https://github.com/USERNAME/fluent-market-island.git
git push -u origin main
```

Ganti `USERNAME` dengan username GitHub milikmu.

## 12. Identitas, atribusi, dan lisensi

Metadata mod menggunakan `Banuarta` sebagai maintainer. Jika username
GitHub berbeda, perbarui metadata `@author` dan identitas maintainer
pada README.

**Lisensi repositori belum ditetapkan.** Jangan menambahkan lisensi MIT
atau lisensi lain sebelum memastikan hak atas kode dasar dan seluruh
komponen yang digunakan. Jika ingin mengirim mod ke koleksi resmi
Windhawk, lengkapi metadata GitHub dan lisensi sesuai aturan repositori
resmi.

## 13. Referensi

Dokumentasi teknis dan sumber eksternal dicatat pada
[REFERENCES.md](REFERENCES.md).
