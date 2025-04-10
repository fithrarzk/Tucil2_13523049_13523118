#ifndef RECONSTRUCT_HPP
#define RECONSTRUCT_HPP

#include <string>
#include "QuadTree.hpp"

void reconstructAndSaveImage(const QuadTree& qt, const std::string& filename);

void reconstructNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int imgHeight, int channels);

bool isValidImage(unsigned char* image, int width, int height, int channels);

void saveFrame(unsigned char* image, int width, int height, int channels, const std::string& folder);

void drawNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int imgHeight, int channels, const std::string& folder);

void reconstructByLevel(const QuadTree& qt, const std::string& gifPath);

#endif