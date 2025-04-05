#include <iostream>
#include <chrono>
#include "header/IO.hpp"
#include "header/QuadTree.hpp"
#include "header/Reconstruct.hpp"
#include "header/Statistics.hpp"
#include "external-libs/stb_image_write.h"
#include "external-libs/stb_image.h"

using namespace std;
using namespace std::chrono;

int main() {
    // 1. Ambil input dari user
    InputParams params = getUserInput();

    // 2. Load gambar
    int width, height, channels;
    unsigned char* image = load_image(params.inputImagePath.c_str(), width, height, channels);
    if (!image) {
        cerr << "Gagal memuat gambar dari: " << params.inputImagePath << endl;
        return 1;
    }

    size_t originalSize = width * height * channels;

    // 3. Bangun pohon quadtree dan ukur waktu
    auto start = high_resolution_clock::now();
    QuadTree qt(image, width, height, channels, params.minBlockSize, params.threshold, params.errorMethod);
    qt.buildTree();
    auto end = high_resolution_clock::now();

    double executionTime = duration<double>(end - start).count();

    // 4. Rekonstruksi dan simpan gambar
    reconstructAndSaveImage(qt, params.outputImagePath);
    printImageSavedMessage(params.outputImagePath);

    // 5. Hitung ukuran kompresi (asumsi ukuran simpul)
    size_t nodeSizeBytes = sizeof(QuadTreeNode);
    size_t compressedSize = qt.getTotalNodes() * nodeSizeBytes;

    // 6. Print statistik
    OutputStats stats {
        executionTime,
        originalSize,
        compressedSize,
        static_cast<double>(originalSize) / compressedSize,
        qt.getMaxDepth(),
        qt.getTotalNodes()
    };

    printOutputStats(stats);

    printGifSavedMessage(params.outputGifPath);

    // 7. Cleanup
    stbi_image_free(image);
    return 0;
}
