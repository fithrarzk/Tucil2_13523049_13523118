#include <iostream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include "header/IO.hpp"
#include "header/QuadTree.hpp"
#include "header/Reconstruct.hpp"
#include "external-libs/stb_image.h"
#include "external-libs/stb_image_write.h"

namespace fs = std::filesystem;
using namespace std;
using namespace std::chrono;

QuadTree findBestThreshold(
    const unsigned char* image,
    int width, int height, int channels,
    int errorMethod,
    double targetRatio,
    const std::string& ext,
    const std::string& finalOutputPath,
    int& bestDepth,
    int& bestTotalNodes,
    const std::string& originalInputPath,
    size_t originalSize
) {
    const std::string tempFolder = "temp_quadtree";
    fs::create_directories(tempFolder);

    double low = 0.0, high = 100.0;
    switch (errorMethod) {
        case 1: // Variance
            high = 20000.0;
            break;
        case 2: // MAD
            high = 255.0;
            break;
        case 3: // Max Pixel Difference
            high = 255.0;
            break;
        case 4: // Entropy
            low = 0.5;
            high = 8.0;
            break;
        case 5: // SSIM
            high = 1.0;
            break;
        default:
            std::cerr << "Metode error tidak valid. Gunakan default range.\n";
            break;
    }
    double bestDiff = 1.0;
    QuadTree bestTree(nullptr, 0, 0, 0, 0, 0, 0);
    std::string bestPath = tempFolder + "/best_output." + ext;
    bool hasBest = false;

    for (int i = 0; i < 10; ++i) {
        double mid = (low + high) / 2.0;
        // cout << "Low : " << low << endl;
        // cout << "High : " << high << endl;
        QuadTree qt(image, width, height, channels, 1, mid, errorMethod);
        qt.buildTree();

        std::string trialPath = tempFolder + "/trial_" + std::to_string(i) + "." + ext;
        reconstructAndSaveImage(qt, trialPath);

        size_t size = fs::file_size(trialPath);
        // cout << "Temp size : " << size << endl;
        double ratio = static_cast<double>(size) / originalSize;
        // cout << "Ratio : " << ratio << endl;
        double diff = abs(ratio - targetRatio);
        // cout << "Diff : " << diff << endl;

        if (diff < bestDiff) {
            bestDiff = diff;
            bestTree = std::move(qt);
            bestDepth = bestTree.getMaxDepth();
            bestTotalNodes = bestTree.getTotalNodes();
            try {
                if (fs::exists(bestPath)) {
                    fs::remove(bestPath);
                }
                fs::copy_file(trialPath, bestPath);
                hasBest = true;
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Gagal menyimpan best output: " << e.what() << std::endl;
            }
        }

        if (ratio > targetRatio) low = mid;
        else high = mid;
    }

    if (hasBest && fs::exists(bestPath)) {
        int w, h, ch;
        unsigned char* data = stbi_load(bestPath.c_str(), &w, &h, &ch, 0);
        if (!data) {
            std::cerr << "Gagal memuat gambar dari bestPath untuk disalin ke output akhir." << std::endl;
        } else {
            bool success = false;
            if (ext == "png") {
                success = stbi_write_png(finalOutputPath.c_str(), w, h, ch, data, w * ch);
            } else if (ext == "jpg" || ext == "jpeg") {
                if (ch == 4) {
                    std::cerr << "Warning: JPEG tidak mendukung alpha channel.\n";
                }
                success = stbi_write_jpg(finalOutputPath.c_str(), w, h, ch, data, 100);
            } else {
                std::cerr << "Format tidak didukung: hanya PNG, JPG, dan JPEG.\n";
            }

            if (!success) {
                std::cerr << "Gagal menulis hasil akhir dengan STB Image.\n";
            }

            stbi_image_free(data);
        }
    }

    try {
        fs::remove_all(tempFolder);
    } catch (...) {}
    
    return bestTree;
}

int main() {
    InputParams params = getUserInput();

    int width, height, channels;
    unsigned char* image = load_image(params.inputImagePath.c_str(), width, height, channels);
    if (!image) {
        cerr << "Gagal memuat gambar dari: " << params.inputImagePath << endl;
        return 1;
    }

    std::string ext = fs::path(params.inputImagePath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (!ext.empty() && ext[0] == '.') ext = ext.substr(1);

    size_t originalSize = fs::file_size(params.inputImagePath);
    QuadTree qt(nullptr, 0, 0, 0, 0, 0.0, 0);
    int maxDepth = 0, totalNodes = 0;

    auto start = high_resolution_clock::now();

    if (params.targetCompression > 0.0) {
        qt = findBestThreshold(
            image, width, height, channels,
            params.errorMethod,
            1-params.targetCompression,
            ext,
            params.outputImagePath,
            maxDepth,
            totalNodes,
            params.inputImagePath,
            originalSize
        );
        maxDepth = qt.getMaxDepth();
        totalNodes = qt.getTotalNodes();
    } else {
        qt = QuadTree(image, width, height, channels, params.minBlockSize, params.threshold, params.errorMethod);
        qt.buildTree();
        reconstructAndSaveImage(qt, params.outputImagePath);
        maxDepth = qt.getMaxDepth();
        totalNodes = qt.getTotalNodes();
    }

    auto end = high_resolution_clock::now();
    auto executionTime = duration_cast<milliseconds>(end - start).count();

    printImageSavedMessage(params.outputImagePath);

    if (!params.outputGifPath.empty()) {
        reconstructByLevel(qt, params.outputGifPath);
    }

    size_t compressedSize = fs::file_size(params.outputImagePath);

    OutputStats stats {
        static_cast<double>(executionTime),
        originalSize,
        compressedSize,
        compressedSize / static_cast<double>(originalSize),
        maxDepth,
        totalNodes
    };

    printOutputStats(stats);
    stbi_image_free(image);
    
    return 0;
}
