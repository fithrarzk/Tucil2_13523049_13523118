#include "external-libs/stb_image_write.h"
#include "header/QuadTree.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <queue>

namespace fs = std::filesystem;

void reconstructNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int imgHeight, int channels) {
    if (node->isLeaf) {
        for (int j = 0; j < node->height; ++j) {
            for (int i = 0; i < node->width; ++i) {
                int x = node->x + i;
                int y = node->y + j;
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

void reconstructAndSaveImage(const QuadTree& qt, const std::string& filename) {
    int width = qt.getWidth();
    int height = qt.getHeight();
    int channels = qt.getChannels();

    unsigned char* outputImage = new unsigned char[width * height * channels];
    reconstructNode(qt.getRoot().get(), outputImage, width, height, channels);

    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    bool success = false;

    if (ext == "png") {
        success = stbi_write_png(filename.c_str(), width, height, channels, outputImage, width * channels);
    } else if (ext == "jpg" || ext == "jpeg") {
        if (channels == 4) {
            std::cerr << "Warning: JPEG tidak mendukung alpha channel." << std::endl;
        }
        success = stbi_write_jpg(filename.c_str(), width, height, channels, outputImage, 100);
    } else {
        std::cerr << "Format tidak didukung (hanya PNG dan JPG)." << std::endl;
    }

    delete[] outputImage;
}

int frameCounter = 0;

bool isValidImage(unsigned char* image, int width, int height, int channels) {
    return image && width > 0 && height > 0 && channels > 0;
}

void saveFrame(unsigned char* image, int width, int height, int channels, const std::string& folder) {
    if (!isValidImage(image, width, height, channels)) return;

    std::ostringstream ss;
    ss << folder << "/frame_" << std::setw(4) << std::setfill('0') << frameCounter++ << ".png";
    std::string filename = ss.str();

    if (!stbi_write_png(filename.c_str(), width, height, channels, image, width * channels)) {
        std::cerr << "Gagal menyimpan frame: " << filename << std::endl;
        std::remove(filename.c_str());
    }
}

void collectNodesPerDepth(QuadTreeNode* node, std::vector<std::vector<QuadTreeNode*>>& levels, int depth) {
    if (!node) return;
    if (depth >= levels.size()) levels.resize(depth + 1);
    levels[depth].push_back(node);
    if (!node->isLeaf) {
        for (const auto& child : node->children) {
            collectNodesPerDepth(child.get(), levels, depth + 1);
        }
    }
}

void reconstructByLevel(const QuadTree& qt, const std::string& gifPath) {
    int width = qt.getWidth();
    int height = qt.getHeight();
    int channels = qt.getChannels();

    if (width <= 0 || height <= 0 || channels <= 0) {
        std::cerr << "Dimensi atau channel gambar tidak valid." << std::endl;
        return;
    }

    unsigned char* image = new unsigned char[width * height * channels];
    std::fill(image, image + width * height * channels, 255);

    fs::path gifFilePath(gifPath);
    fs::path gifDir = gifFilePath.parent_path();
    std::string frameFolder = (gifDir / "frames").string();

    try {
        fs::create_directories(frameFolder);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Gagal membuat folder frame: " << e.what() << std::endl;
        delete[] image;
        return;
    }

    frameCounter = 0;

    std::queue<QuadTreeNode*> q;
    q.push(qt.getRoot().get());

    while (!q.empty()) {
        int levelSize = q.size();

        for (int i = 0; i < levelSize; ++i) {
            QuadTreeNode* node = q.front(); q.pop();

            std::vector<uint8_t>& color = node->color;

            for (int j = 0; j < node->height; ++j) {
                for (int i = 0; i < node->width; ++i) {
                    int x = node->x + i;
                    int y = node->y + j;
                    if (x < width && y < height) {
                        int idx = (y * width + x) * channels;
                        for (int c = 0; c < channels; ++c) {
                            image[idx + c] = color[c];
                        }
                    }
                }
            }

            if (!node->isLeaf) {
                for (const auto& child : node->children) {
                    if (child) q.push(child.get());
                }
            }
        }

        saveFrame(image, width, height, channels, frameFolder);
    }

    std::string command = "magick -delay 50 -loop 0 \"" + frameFolder + "/frame_*.png\" \"" + gifPath + "\"";
    std::cout << "Membuat GIF: " << command << std::endl;
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Gagal membuat GIF. Pastikan ImageMagick tersedia." << std::endl;
    } else {
        std::cout << "GIF disimpan: " << gifPath << std::endl;
        try {
            fs::remove_all(frameFolder);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Gagal menghapus folder frame: " << e.what() << std::endl;
        }
    }

    delete[] image;
}


