#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <vector>
#include <memory>
#include <cstdint>
#include <string>
#include <map>
#include "ErrorCalculator.hpp"

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

class QuadTree {
public:
    QuadTree(const unsigned char* image, int width, int height, int channels,
             int minBlockSize, double threshold, int errorMethod);
    void buildTree();
    const std::unique_ptr<QuadTreeNode>& getRoot() const;
    int getMaxDepth() const;
    int getTotalNodes() const;
    int getWidth() const;
    int getHeight() const;
    int getChannels() const;

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

    void collectNodesPerDepth(std::vector<std::vector<QuadTreeNode*>>& levels);

    void collectNodesByDepth(std::map<int, std::vector<QuadTreeNode*>>& nodesByDepth);

    // Ambil blok piksel dari gambar
    std::vector<uint8_t> getBlock(int x, int y, int width, int height);

    // Hitung warna rata-rata dari blok piksel
    std::vector<uint8_t> calculateAverageColor(const std::vector<uint8_t>& block);

    // Hitung error antara blok dan warna rata-rata
    double calculateError(const std::vector<uint8_t>& block, const std::vector<uint8_t>& avgColor, int width, int height);
};

#endif