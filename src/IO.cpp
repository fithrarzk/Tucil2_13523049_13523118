#include "header/IO.hpp"
#include <iostream>
#include <limits>
#include "external-libs/stb_image.h"
#include "external-libs/stb_image_write.h"

using namespace std;

// Tampilkan daftar metode error yang tersedia
void printErrorMethodOptions() {
    cout << "Pilih metode error:\n";
    cout << "1 = Variance (>0)\n";
    cout << "2 = MAD (Mean Absolute Deviation) (0-255)\n";
    cout << "3 = Max Pixel Difference (0-255)\n";
    cout << "4 = Entropy (0-8)\n";
    cout << "5 = SSIM (0-1)\n";
}

InputParams getUserInput() {
    InputParams params;

    cout << "Masukkan path gambar input (absolute path): ";
    getline(cin, params.inputImagePath);

    // Validasi metode error
    printErrorMethodOptions();
    while (true) {
        cout << "Pilihanmu (1-5): ";
        cin >> params.errorMethod;
        if (params.errorMethod >= 1 && params.errorMethod <= 5) break;
        cout << "Metode tidak valid! Silakan pilih antara 1 sampai 5.\n";
    }

    cout << "Masukkan threshold: ";
    cin >> params.threshold;

    cout << "Masukkan ukuran blok minimum: ";
    cin >> params.minBlockSize;

    cout << "Masukkan target persentase kompresi (0 = nonaktifkan mode ini): ";
    cin >> params.targetCompression;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Masukkan path untuk gambar output (absolute path): ";
    getline(cin, params.outputImagePath);

    cout << "Masukkan path untuk menyimpan GIF (opsional, tekan enter jika tidak ingin): ";
    getline(cin, params.outputGifPath);

    return params;
}

void printOutputStats(const OutputStats& stats) {
    cout << "\n===== Statistik Kompresi =====\n";
    cout << "Waktu eksekusi          : " << stats.executionTime << " ms\n";
    cout << "Ukuran gambar sebelum   : " << stats.originalSize << " bytes\n";
    cout << "Ukuran gambar setelah   : " << stats.compressedSize << " bytes\n";
    cout << "Persentase kompresi     : " << (1-stats.compressionRatio)*100 << "%\n";
    cout << "Kedalaman pohon         : " << stats.maxDepth << "\n";
    cout << "Jumlah simpul pohon     : " << stats.totalNodes << "\n";
}

void printImageSavedMessage(const string& path) {
    cout << "Gambar hasil kompresi disimpan di: " << path << "\n";
}

void printGifSavedMessage(const string& path) {
    if (!path.empty()) {
        cout << "GIF proses kompresi disimpan di: " << path << "\n";
    }
}

unsigned char* load_image(const char* filename, int& width, int& height, int& channels) {
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        cerr << "Gagal memuat gambar dari: " << filename << endl;
        exit(1);  // keluar dari program, karena input tidak valid
    }
    return data;
}