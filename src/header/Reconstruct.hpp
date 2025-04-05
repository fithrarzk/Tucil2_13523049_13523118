#ifndef RECONSTRUCT_HPP
#define RECONSTRUCT_HPP

#include <string>
#include "QuadTree.hpp"

// Fungsi untuk merekonstruksi gambar dari QuadTree dan menyimpannya ke file PNG
void reconstructAndSaveImage(const QuadTree& qt, const std::string& filename);

#endif
