#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include "QuadTree.hpp"
#include <string>

class Statistics {
public:
    static void printStatistics(const QuadTree& tree, int originalSizeBytes, int compressedSizeBytes);
};

#endif
