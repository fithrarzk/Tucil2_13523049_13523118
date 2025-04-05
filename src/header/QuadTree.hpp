#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <vector>
#include <memory>
#include <cstdint>
#include "ErrorCalculator.hpp"

// Struktur node pohon kuad untuk representasi area gambar
struct QuadTreeNode {
    int x, y;                     // Koordinat kiri atas blok
    int width, height;           // Ukuran blok
    bool isLeaf;                 // Apakah node ini daun (tidak dibagi lagi)
    std::vector<uint8_t> color;  // Warna rata-rata blok (RGB)
    std::unique_ptr<QuadTreeNode> children[4]; // Child nodes (TL, TR, BL, BR)

   QuadTreeNode(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height), isLeaf(true), color{} {
    for (int i = 0; i < 4; ++i) {
        children[i] = nullptr;
    }
}
};

// Kelas QuadTree untuk membangun dan menyimpan pohon kuad dari gambar
class QuadTree {
public:
    QuadTree(const unsigned char* image, int width, int height, int channels,
             int minBlockSize, double threshold, int errorMethod);

    // Bangun pohon quadtree dari gambar
    void buildTree();

    // Akses akar pohon
    const std::unique_ptr<QuadTreeNode>& getRoot() const;

    // Dapatkan kedalaman maksimum pohon
    int getMaxDepth() const;

    // Dapatkan jumlah total node dalam pohon
    int getTotalNodes() const;

private:
    std::unique_ptr<QuadTreeNode> root;
    const unsigned char* image;
    int imgWidth, imgHeight, channels;
    int minBlockSize;
    double threshold;
    int errorMethod;
    int maxDepth;
    int totalNodes;

    // Rekursif membangun pohon dari area (x, y) dengan ukuran width x height
    std::unique_ptr<QuadTreeNode> build(int x, int y, int width, int height, int depth);

    // Ambil blok piksel dari gambar
    std::vector<uint8_t> getBlock(int x, int y, int width, int height);

    // Hitung warna rata-rata dari blok piksel
    std::vector<uint8_t> calculateAverageColor(const std::vector<uint8_t>& block);

    // Hitung error antara blok dan warna rata-rata
    double calculateError(const std::vector<uint8_t>& block, const std::vector<uint8_t>& avgColor);
};

#endif
