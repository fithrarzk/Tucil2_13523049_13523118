<h1 align="center"> Tugas Kecil 2 IF2211 Strategi Algoritma </h1>
<h1 align="center"> Kompresi Gambar Dengan Metode Quadtree </h1>


## General Information
Program ini mengimplementasikan algoritma Divide and Conquer dalam bentuk metode kompresi gambar berbasis struktur data Quadtree. Dengan metode ini, gambar akan dipecah menjadi blok-blok persegi kecil berdasarkan seberapa seragam warna di blok tersebut. Jika blok dinyatakan cukup seragam sesuai ambang batas (threshold) dan metode error tertentu, maka blok disimpan sebagai simpul daun dengan warna rata-rata. Program ini mendukung berbagai metode error seperti Variance, Mean Absolute Deviation (MAD), Max Pixel Difference, Entropy, dan SSIM. Output program berupa gambar hasil kompresi dan visualisasi proses kompresi dalam bentuk GIF.


## Technology Used
- C++17
- STB Image Library (stb_image, stb_image_write)
- ImageMagick
- Filesystem C++17
- G++14


## Contributors
|   NIM    |                  Nama                  |
| :------: | :------------------------------------: |
| 13522049 |         Muhammad Fithra Rizki          |
| 13522118 |          Farrel Athalla Putra          |



## Project Structure
```bash
.
│   README.md
│   Makefile
│
├───bin                                 # File binary hasil kompilasi
│
├───doc
│   └─── Tucil2_13523049_13523118.pdf   # Berisi laporan akhir
│
├───src
│   ├─── header                         # Header files
│   │    ├─── IO.hpp
│   │    ├─── QuadTree.hpp
│   │    ├─── Reconstruct.hpp
│   │    └─── ErrorCalculator.hpp
│   ├─── external-libs                  # STB Image Library
│   │    ├─── stb_image.h
│   │    └─── stb_image_write.h
│   │    └─── stb_img.cpp
│   ├─── IO.cpp
│   ├─── QuadTree.cpp
│   ├─── Reconstruct.cpp
│   ├─── ErrorCalculator.cpp
│   └─── main.cpp                       # Entry point program
│
└───test                                # Folder untuk pengujian
```


## How to Run
1. Clone repository ini dengan mengetikkan `git clone https://github.com/fithrarzk/Tucil2_13523049_13523118` pada terminal.
2. Pindah ke directory src dengan `cd src`.
3. Jalankan `g++ -std=c++17 -O2 -Wall main.cpp IO.cpp ErrorCalculator.cpp QuadTree.cpp Reconstruct.cpp external-libs/stb_img.cpp -I./header -I./external-libs -o image_compressor.exe` pada terminal untuk kompilasi.
3. Run file dengan `./image_compressor.exe`.
4. Input absolute path gambar yang ingin dikompresi.
5. Input metode error yang ingin digunakan.
6. Input threshold yang digunakan (>=0).
7. Input ukuran blok minimum (>=0).
8. Input target persentase kompresi (0-1), jika tidak ingin menggunakan target kompresi, input 0.
9. Input absolute path output untuk gambar hasil kompresi.
10. Input absolute path output untuk gif proses quadtree, langsung enter jika tidak ingin menyimpan gif.
11. Jika semua input sudah benar, maka tunggu prosesnya dan statistik dari kompresi akan muncul ketika proses sudah selesai.


## Additional Notes
Program ini dibuat dan dites menggunakan C++17 dan G++14.
Jika Anda belum menginstal library yang diperlukan, silakan instal terlebih dahulu menggunakan perintah berikut.
|    Library       |                                 Cara Install                                                                   |
| :--------------: | :------------------------------------------------------------------------------------------------------------: |
| STB(header-only) | terdapat pada folder `external-libs`                                                                           |
| Imagemagick      | `brew install imagemagick` untuk Mac, `sudo apt install imagemagick` untuk Linux, atau install melalui browser |
