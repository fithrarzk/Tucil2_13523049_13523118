#include "external-libs/stb_image_write.h"
#include "header/QuadTree.hpp"
#include <iostream>

// Fungsi rekonstruksi dari QuadTree
void reconstructNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int channels) {
    if (node->isLeaf) {
        for (int j = 0; j < node->height; ++j) {
            for (int i = 0; i < node->width; ++i) {
                int idx = ((node->y + j) * imgWidth + (node->x + i)) * channels;
                for (int c = 0; c < channels; ++c) {
                    output[idx + c] = node->color[c];
                }
            }
        }
    } else {
        for (const auto& child : node->children) {
            if (child) {
                reconstructNode(child.get(), output, imgWidth, channels);
            }
        }
    }
}

// Wrapper untuk reconstruct dan simpan gambar
void reconstructAndSaveImage(const QuadTree& qt, const std::string& filename) {
    int width = qt.getRoot()->width;
    int height = qt.getRoot()->height;
    int channels = qt.getRoot()->color.size(); // Ambil dari leaf, asumsinya semua node pakai channel yg sama

    unsigned char* outputImage = new unsigned char[width * height * channels];

    reconstructNode(qt.getRoot().get(), outputImage, width, channels);

    if (stbi_write_png(filename.c_str(), width, height, channels, outputImage, width * channels)) {
        std::cout << "Image saved to: " << filename << std::endl;
    } else {
        std::cerr << "Failed to save image!" << std::endl;
    }

    delete[] outputImage;
}
