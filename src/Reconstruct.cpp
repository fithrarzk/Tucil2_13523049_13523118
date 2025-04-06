#include "external-libs/stb_image_write.h"
#include "header/QuadTree.hpp"
#include <iostream>
#include <algorithm>

void reconstructNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int imgHeight, int channels) {
    if (node->isLeaf) {
        for (int j = 0; j < node->height; ++j) {
            for (int i = 0; i < node->width; ++i) {
                int x = node->x + i;
                int y = node->y + j;

                // Cegah akses di luar batas gambar
                if (x < imgWidth && y < imgHeight) {
                    int idx = (y * imgWidth + x) * channels;
                    for (int c = 0; c < channels; ++c) {
                        output[idx + c] = node->color[c];
                    }
                }
            }
        }
    } else {
        for (const auto& child : node->children) {
            if (child) {
                reconstructNode(child.get(), output, imgWidth, imgHeight, channels);
            }
        }
    }
}


// Wrapper untuk reconstruct dan simpan gambar
void reconstructAndSaveImage(const QuadTree& qt, const std::string& filename) {
    int width = qt.getWidth();
    int height = qt.getHeight();
    int channels = qt.getChannels();

    unsigned char* outputImage = new unsigned char[width * height * channels];
    reconstructNode(qt.getRoot().get(), outputImage, width, height, channels);
    int center = ((height / 2) * width + (width / 2)) * channels;
    std::cout << "Sample pixel: ";
    for (int c = 0; c < channels; ++c) std::cout << (int)outputImage[center + c] << " ";
    std::cout << std::endl;

    // Ambil ekstensi file output
    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); // lowercase biar aman

    bool success = false;

    if (ext == "png") {
        success = stbi_write_png(filename.c_str(), width, height, channels, outputImage, width * channels);
    } else if (ext == "jpg" || ext == "jpeg") {
        if (channels == 4) {
            std::cerr << "Warning: JPEG tidak mendukung alpha channel (transparansi). Output mungkin tidak akurat." << std::endl;
        }
        success = stbi_write_jpg(filename.c_str(), width, height, channels, outputImage, 100); // 100 = kualitas maksimal
    } else {
        std::cerr << "Format gambar tidak didukung (hanya PNG dan JPG/JPEG)." << std::endl;
    }

    delete[] outputImage;
}
