# References and source notes

Dokumen ini membedakan dokumentasi API yang benar-benar digunakan
sebagai landasan teknis dengan endpoint data eksternal yang dipanggil
oleh kode.

## A. Windhawk

1.  **Windhawk — Creating a new mod**  
    <https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod>  
    Rujukan untuk format metadata mod (`@id`, `@name`, `@version`,
    `@author`, `@include`, `@compilerOptions`), blok README, format YAML
    pengaturan, serta proses kompilasi C++ dalam Windhawk.

2.  **Windhawk — Mods as tools: Running mods in a dedicated process**  
    <https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process>  
    Rujukan pola launcher tool-mod yang digunakan kode untuk menjalankan
    widget pada proses Windhawk khusus. Kode ini menyertakan
    implementasi launcher di bagian akhir source.

3.  **Windhawk — Official website**  
    <https://windhawk.net/>  
    Sumber unduhan dan informasi produk Windhawk.

4.  **Windhawk Mods — Official repository README**  
    <https://github.com/ramensoftware/windhawk-mods/blob/main/README.md>  
    Rujukan format pengajuan mod ke koleksi resmi Windhawk. Pengajuan
    resmi memiliki persyaratan file, metadata GitHub, dan lisensi.

## B. Microsoft Windows APIs

5.  **DesktopWindowTarget Class**  
    <https://learn.microsoft.com/en-us/uwp/api/windows.ui.composition.desktop.desktopwindowtarget>  
    Dokumentasi target visual Windows Composition untuk jendela desktop.

6.  **Using the Visual Layer with Win32**  
    <https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/using-the-visual-layer-with-win32>  
    Panduan Microsoft untuk menghubungkan Compositor/Composition visual
    ke jendela Win32.

7.  **UpdateLayeredWindow function**  
    <https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-updatelayeredwindow>  
    Rujukan untuk memperbarui posisi, ukuran, bentuk, konten, dan
    translucency layered window.

8.  **SetLayeredWindowAttributes function**  
    <https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setlayeredwindowattributes>  
    Rujukan alpha global pada layered window; alpha 0 berarti transparan
    dan 255 berarti opaque.

9.  **WinHTTP — WinHttpOpen**  
    <https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpopen>  
    Dokumentasi inisialisasi sesi WinHTTP yang digunakan untuk
    permintaan HTTPS.

10. **WinHTTP — WinHttpConnect**  
    <https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpconnect>  
    Dokumentasi koneksi ke host server HTTP.

11. **GDI+ — Graphics::FillPath**  
    <https://learn.microsoft.com/en-us/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-fillpath>  
    Rujukan untuk mengisi area graphics path dengan brush.

12. **GDI+ — Drawing Text**  
    <https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-drawing-text-use>  
    Rujukan untuk menggambar teks menggunakan Graphics, Font, dan Brush.

13. **GDI+ — Clipping**  
    <https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-clipping-about>  
    Rujukan pembatasan area gambar menggunakan clipping region.

14. **Microsoft Windows.UI.Composition Win32 samples**  
    <https://github.com/microsoft/Windows.UI.Composition-Win32-Samples>  
    Contoh implementasi Windows Composition pada aplikasi desktop Win32.

## C. Market-data endpoint and usage restrictions

15. **Yahoo Finance chart endpoint used by this source**  
    <https://query1.finance.yahoo.com/v8/finance/chart/>  
    Kode membentuk permintaan chart untuk setiap simbol dengan path
    `/v8/finance/chart/{symbol}?range=1d&interval=1m`. Ini adalah
    endpoint web yang dipakai kode, bukan dokumentasi API publik resmi.

16. **Yahoo Finance search endpoint used by this source**  
    <https://query1.finance.yahoo.com/v1/finance/search>  
    Kode mengirim kata kunci pencarian ke endpoint ini. Endpoint ini
    juga tidak diperlakukan sebagai API publik resmi yang memiliki
    kontrak stabil.

17. **Yahoo Terms of Service**  
    <https://legal.yahoo.com/us/en/yahoo/terms/otos/index.html>  
    Ketentuan Yahoo memuat pembatasan terhadap pengumpulan data otomatis
    tanpa izin sebelumnya serta penggunaan ulang konten/data tertentu.
    Periksa ketentuan yang berlaku di wilayah pengguna dan aturan produk
    terkait sebelum mengoperasikan atau mendistribusikan fitur
    pengambilan data.

18. **Tiingo — Yahoo Finance API: What It Is, How to Access It, and a
    Compliant Alternative**  
    <https://www.tiingo.com/blog/yahoo-finance-api/>  
    Sumber pihak ketiga yang membahas status endpoint Yahoo Finance
    sebagai akses tidak resmi/tidak terdokumentasi dan risiko perubahan,
    pembatasan, serta lisensi data. Ini bukan pernyataan resmi dari
    Yahoo.

## D. Attribution and scope

- Dokumentasi Microsoft dan Windhawk menjadi rujukan untuk penggunaan
  API dan format mod; bukan berarti kode ini disalin dari satu contoh
  Microsoft.
- Endpoint Yahoo Finance dicatat sebagai layanan eksternal yang
  dipanggil oleh source, bukan sebagai dependensi resmi yang dijamin.
- Source repository berasal dari file mod yang diberikan pemilik proyek
  dan kemudian direvisi dengan bantuan AI. `Banuarta` dicantumkan sebagai
  maintainer proyek.
- Tidak ada lisensi perangkat lunak yang ditambahkan dalam paket ini
  karena hak dan lisensi kode dasar perlu dipastikan terlebih dahulu.
