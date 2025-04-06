#include "header/QuadTree.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>

QuadTree::QuadTree(const unsigned char* img, int width, int height, int channels,
                   int minBlockSize, double threshold, int errorMethod)
    : image(img), imgWidth(width), imgHeight(height), channels(channels),
      minBlockSize(minBlockSize), threshold(threshold), errorMethod(errorMethod),
      maxDepth(0), totalNodes(0) {}


void QuadTree::buildTree() {
    root = build(0, 0, imgWidth, imgHeight, 0);

}

std::vector<uint8_t> QuadTree::getBlock(int x, int y, int width, int height) {
    std::vector<uint8_t> block(width * height * channels);
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            for (int c = 0; c < channels; ++c) {
                block[(j * width + i) * channels + c] = image[((y + j) * imgWidth + (x + i)) * channels + c];
            }
        }
    }

    return block;
}

std::vector<uint8_t> QuadTree::calculateAverageColor(const std::vector<uint8_t>& block) {
    std::vector<uint64_t> sum(channels, 0);
    int totalPixels = block.size() / channels;
    for (size_t i = 0; i < block.size(); ++i) {
        sum[i % channels] += block[i];
    }
    std::vector<uint8_t> avg(channels);
    // std::cout << "Rata-rata warna: ";
    // for (uint8_t c : avg) std::cout << (int)c << " ";
    // std::cout << std::endl;
    for (int c = 0; c < channels; ++c) {
        avg[c] = static_cast<uint8_t>(sum[c] / totalPixels);
    }

    return avg;
}

double QuadTree::calculateError(const std::vector<uint8_t>& block, const std::vector<uint8_t>& avgColor, int width, int height) {
    // Untuk metode MAD, VARIANCE, ENTROPY, bandingkan block dengan dirinya sendiri
    if (errorMethod == MAD_METHOD || errorMethod == VARIANCE_METHOD || errorMethod == ENTROPY_METHOD) {
        return ErrorCalculator::calculateError(errorMethod, block, block, width, height);
    }

    // Untuk metode lain (MAE, MAXDIFF, SSIM), buat avgBlock
    std::vector<uint8_t> avgBlock(block.size());
    for (size_t i = 0; i < block.size(); i += channels) {
        for (int c = 0; c < channels; ++c) {
            avgBlock[i + c] = avgColor[c];
        }
    }
    return ErrorCalculator::calculateError(errorMethod, block, avgBlock, width, height);
}



std::unique_ptr<QuadTreeNode> QuadTree::build(int x, int y, int width, int height, int depth) {
    auto node = std::make_unique<QuadTreeNode>(x, y, width, height);
    ++totalNodes;
    maxDepth = std::max(maxDepth, depth);

    auto block = getBlock(x, y, width, height);
    auto avgColor = calculateAverageColor(block);
    double error = calculateError(block, avgColor, width, height);

    // std::cout << "Error at (" << x << "," << y << "), size = (" << width << "x" << height 
    //       << "), error = " << error << std::endl;

    if (width <= minBlockSize || height <= minBlockSize || error <= threshold) {
        node->isLeaf = true;
        node->color = avgColor;
        return node;
    }

    node->isLeaf = false;
    int halfW = width / 2;
    int halfH = height / 2;

    node->children[0] = build(x, y, halfW, halfH, depth + 1);                  // Top-left
    node->children[1] = build(x + halfW, y, width - halfW, halfH, depth + 1);  // Top-right
    node->children[2] = build(x, y + halfH, halfW, height - halfH, depth + 1); // Bottom-left
    node->children[3] = build(x + halfW, y + halfH, width - halfW, height - halfH, depth + 1); // Bottom-right

    return node;
}

int QuadTree::getMaxDepth() const {
    return maxDepth;
}

int QuadTree::getTotalNodes() const {
    return totalNodes;
}

const std::unique_ptr<QuadTreeNode>& QuadTree::getRoot() const {
    return root;
}

int QuadTree::getWidth() const { return imgWidth; }
int QuadTree::getHeight() const { return imgHeight; }
int QuadTree::getChannels() const { return channels; }