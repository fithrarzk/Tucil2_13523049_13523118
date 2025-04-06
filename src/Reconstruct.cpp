#include "external-libs/stb_image_write.h"
#include "header/QuadTree.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <regex>
namespace fs = std::filesystem;

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

    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); 

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


int frameCounter = 0;
const int minPixelChange = 500;

// Validasi apakah data image kosong
bool isValidImage(unsigned char* image, int width, int height, int channels) {
    return image != nullptr && width > 0 && height > 0 && channels > 0;
}

// Menyimpan 1 frame dan pastikan hasilnya valid
void saveFrame(unsigned char* image, int width, int height, int channels, const std::string& folder) {
    if (!isValidImage(image, width, height, channels)) {
        std::cerr << "Data frame tidak valid, tidak disimpan." << std::endl;
        return;
    }

    std::ostringstream ss;
    ss << folder << "/frame_" << std::setw(4) << std::setfill('0') << frameCounter++ << ".png";
    std::string filename = ss.str();

    bool success = stbi_write_png(filename.c_str(), width, height, channels, image, width * channels);
    if (!success) {
        std::cerr << "Gagal menyimpan frame: " << filename << std::endl;
        std::remove(filename.c_str());  // Hapus file kalau rusak
    }
}

// Rekonstruksi + simpan per node untuk GIF
int drawNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int imgHeight, int channels, const std::string& folder) {
    int totalChangedPixels = 0;

    if (node->isLeaf) {
        for (int j = 0; j < node->height; ++j) {
            for (int i = 0; i < node->width; ++i) {
                int x = node->x + i;
                int y = node->y + j;
                if (x < imgWidth && y < imgHeight) {
                    int idx = (y * imgWidth + x) * channels;
                    bool changed = false;
                    for (int c = 0; c < channels; ++c) {
                        if (output[idx + c] != node->color[c]) {
                            output[idx + c] = node->color[c];
                            changed = true;
                        }
                    }
                    if (changed) totalChangedPixels++;
                }
            }
        }

        // Simpan frame hanya jika perubahan besar
        if (totalChangedPixels >= minPixelChange) {
            saveFrame(output, imgWidth, imgHeight, channels, folder);
        }
    } else {
        for (const auto& child : node->children) {
            if (child) {
                totalChangedPixels += drawNode(child.get(), output, imgWidth, imgHeight, channels, folder);
            }
        }
    }

    return totalChangedPixels;
}

void reconstructAndSaveWithGif(const QuadTree& qt, const std::string& gifPath) {
    int width = qt.getWidth();
    int height = qt.getHeight();
    int channels = qt.getChannels();

    if (width <= 0 || height <= 0 || channels <= 0) {
        std::cerr << "Dimensi atau channel gambar tidak valid." << std::endl;
        return;
    }

    unsigned char* outputImage = new unsigned char[width * height * channels];
    std::fill(outputImage, outputImage + width * height * channels, 255); // putih

    fs::path gifFilePath(gifPath);
    fs::path gifDir = gifFilePath.parent_path();
    std::string frameFolder = (gifDir / "frames").string();

    fs::create_directories(frameFolder);
    frameCounter = 0;

    drawNode(qt.getRoot().get(), outputImage, width, height, channels, frameFolder);

    std::string command = "magick -delay 10 -loop 0 " + frameFolder + "/frame_*.png " + gifPath;
    std::cout << "Membuat GIF: " << command << std::endl;
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Gagal membuat GIF. Pastikan 'magick' dari ImageMagick tersedia dan input PNG valid." << std::endl;
    } else {
        std::cout << "GIF proses disimpan sebagai " << gifPath << std::endl;

        // Hapus folder frames setelah GIF dibuat
        try {
            fs::remove_all(frameFolder);
            std::cout << "Folder frame sementara dihapus: " << frameFolder << std::endl;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Gagal menghapus folder frame: " << e.what() << std::endl;
        }
    }

    delete[] outputImage;
}
