#include "header/QuadTree.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <queue>
#include <map>
#include <functional>

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
    for (int c = 0; c < channels; ++c) {
        avg[c] = static_cast<uint8_t>(sum[c] / totalPixels);
    }

    return avg;
}

double QuadTree::calculateError(const std::vector<uint8_t>& block, const std::vector<uint8_t>& avgColor, int width, int height) {
    if (errorMethod == MAD_METHOD || errorMethod == VARIANCE_METHOD || errorMethod == ENTROPY_METHOD) {
        return ErrorCalculator::calculateError(errorMethod, block, block, width, height);
    }

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
    node->color = avgColor;

    if (width <= minBlockSize || height <= minBlockSize || error <= threshold) {
        node->isLeaf = true;
        return node;
    }
    // node->color = avgColor;
    // if (width <= minBlockSize || height <= minBlockSize) {
    //     node->isLeaf = true;
    //     return node;
    // }

    node->isLeaf = false;
    int halfW = width / 2;
    int halfH = height / 2;

    node->children[0] = build(x, y, halfW, halfH, depth + 1);                  // Top-left
    node->children[1] = build(x + halfW, y, width - halfW, halfH, depth + 1);  // Top-right
    node->children[2] = build(x, y + halfH, halfW, height - halfH, depth + 1); // Bottom-left
    node->children[3] = build(x + halfW, y + halfH, width - halfW, height - halfH, depth + 1); // Bottom-right

    return node;
}

void QuadTree::collectNodesPerDepth(std::vector<std::vector<QuadTreeNode*>>& levels) {
    if (!root) return;

    std::queue<std::pair<QuadTreeNode*, int>> q;
    q.push({ root.get(), 0 });

    while (!q.empty()) {
        auto [node, depth] = q.front(); q.pop();

        if (levels.size() <= depth) {
            levels.emplace_back();
        }
        levels[depth].push_back(node);

        if (!node->isLeaf) {
            for (auto& child : node->children) {
                if (child) {
                    q.push({ child.get(), depth + 1 });
                }
            }
        }
    }
}

void QuadTree::collectNodesByDepth(std::map<int, std::vector<QuadTreeNode*>>& nodesByDepth) {
    std::function<void(QuadTreeNode*, int)> dfs = [&](QuadTreeNode* node, int depth) {
        if (!node) return;
        nodesByDepth[depth].push_back(node);
        if (node->isLeaf) return;
        for (auto& child : node->children) {
            if (child) dfs(child.get(), depth + 1);
        }
    };
    dfs(root.get(), 0);
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
