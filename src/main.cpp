#include <iostream>
#include <chrono>
#include <filesystem>
#include "header/IO.hpp"
#include "header/QuadTree.hpp"
#include "header/Reconstruct.hpp"
#include "external-libs/stb_image_write.h"
#include "external-libs/stb_image.h"

namespace fs = std::filesystem;
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

    size_t originalSize = fs::file_size(params.inputImagePath);

    // 3. Ini buat quadtree rel
    auto start = high_resolution_clock::now();
    QuadTree qt(image, width, height, channels, params.minBlockSize, params.threshold, params.errorMethod);
    qt.buildTree();
    auto end = high_resolution_clock::now();

    double executionTime = duration_cast<milliseconds>(end - start).count();

    // 4. Ini rekonstruksi gambarnya sama gif rel
    reconstructAndSaveImage(qt, params.outputImagePath);
    printImageSavedMessage(params.outputImagePath);
    if (!params.outputGifPath.empty()) {
        reconstructByLevel(qt, params.outputGifPath);
    }

    // 5. Hitung ukuran kompresi (asumsi ukuran simpul)
    size_t nodeSizeBytes = sizeof(QuadTreeNode);
    size_t compressedSize = fs::file_size(params.outputImagePath);

    // 6. print
    OutputStats stats {
        executionTime,
        originalSize,
        compressedSize,
        compressedSize/static_cast<double>(originalSize),
        qt.getMaxDepth(),
        qt.getTotalNodes()
    };

    printOutputStats(stats);

    // 7. Cleanup
    stbi_image_free(image);
    return 0;
}