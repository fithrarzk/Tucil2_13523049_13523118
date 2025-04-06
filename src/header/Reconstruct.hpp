#ifndef RECONSTRUCT_HPP
#define RECONSTRUCT_HPP

#include <string>
#include "QuadTree.hpp"

// Fungsi untuk merekonstruksi gambar dari QuadTree dan menyimpannya ke file PNG
void reconstructAndSaveImage(const QuadTree& qt, const std::string& filename);

void reconstructNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int imgHeight, int channels);

bool isValidImage(unsigned char* image, int width, int height, int channels);

void saveFrame(unsigned char* image, int width, int height, int channels, const std::string& folder);

void drawNode(QuadTreeNode* node, unsigned char* output, int imgWidth, int imgHeight, int channels, const std::string& folder);

void reconstructAndSaveWithGif(const QuadTree& qt, const std::string& gifPath);

#endif
