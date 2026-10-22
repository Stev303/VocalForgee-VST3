# VocalForge MultiFX — project starter VST3

Project C++ JUCE untuk Windows/FL Studio. Ini adalah **starter/MVP source**, bukan binary VST3 yang sudah dikompilasi.

## Status fitur
- Megaphone: band-pass 300 Hz–4 kHz + saturasi, implementasi awal.
- Robot: ring modulation, implementasi awal.
- Doubler: delay stereo dasar, implementasi awal.
- Chipmunk: kontrol tersedia, tetapi DSP saat ini hanya pendekatan demonstrasi dan BUKAN pitch shifter berkualitas produksi.
- Harmony: tombol/parameter tersedia, tetapi saat ini hanya placeholder ring-modulated; BELUM mendeteksi pitch atau membuat harmoni diatonik.
- UI, parameter automation, dan save/restore state: fondasi tersedia.

## Prasyarat
1. Windows 10/11.
2. Visual Studio 2022 dengan workload **Desktop development with C++**.
3. CMake.
4. JUCE (unduh/clone dari https://juce.com/download/ atau repositori resmi).
5. FL Studio untuk pengujian.

## Susunan folder
Letakkan checkout JUCE di `VocalForge/JUCE`, sehingga `JUCE/CMakeLists.txt` ada.

## Build
Buka Developer PowerShell for VS 2022 pada folder project:
```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```
Jika CMake tidak menemukan JUCE, pastikan folder `JUCE` berada di lokasi yang benar. Output VST3 biasanya berada di `build/VocalForge_artefacts/Release/VST3/`.

Salin folder `VocalForge MultiFX.vst3` ke:
`C:\Program Files\Common Files\VST3\`
Kemudian buka FL Studio dan lakukan rescan plugin.

## Penting sebelum dipakai untuk rekaman/dirilis
- Ganti chipmunk demo dengan pitch shifter duration-preserving dan formant shifter yang sungguhan.
- Ganti harmony placeholder dengan pitch detector (YIN/autocorrelation), scale mapper, dan pitch shifter per voice.
- Pindahkan seluruh state oscillator ke anggota class (sudah ada sebagai anggota di starter).
- Tambahkan parameter smoothing, latency reporting, oversampling/anti-aliasing, denormal/NaN checks, automated tests, dan validasi mono/stereo.
- Uji CPU, latency, automation, preset save/restore, dan semua kombinasi efek.
- Periksa lisensi JUCE dan setiap library DSP eksternal sebelum distribusi komersial.

Jangan menganggap ini plugin final untuk produksi sampai semua tes tersebut lulus.


## Opsi build otomatis di Windows melalui GitHub Actions
File `.github/workflows/build-windows.yml` disertakan untuk menjalankan build di runner Windows:
1. Buat repository GitHub baru dan upload seluruh isi folder project ini (termasuk `.github`).
2. Buka tab **Actions**, pilih **Build Windows VST3**, lalu tekan **Run workflow**.
3. Setelah selesai, unduh artifact **VocalForge-Windows-VST3** dari halaman run tersebut.
4. Ekstrak artifact dan salin folder `.vst3` ke `C:\\Program Files\\Common Files\\VST3\\`.

Workflow ini membantu proses kompilasi, tetapi tidak menjadikan algoritma Chipmunk atau Harmony sebagai DSP final. Build perlu diperiksa hasilnya dan efek pitch/harmony masih perlu implementasi lanjutan.
