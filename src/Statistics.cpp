#include "header/Statistics.hpp"
#include <iostream>
#include <iomanip>

void Statistics::printStatistics(const QuadTree& tree, int originalSizeBytes, int compressedSizeBytes) {
    int maxDepth = tree.getMaxDepth();
    int totalNodes = tree.getTotalNodes();

    std::cout << "===== Compression Statistics =====\n";
    std::cout << "Original size       : " << originalSizeBytes << " bytes\n";
    std::cout << "Compressed estimate : " << compressedSizeBytes << " bytes\n";
    std::cout << "Max depth of tree   : " << maxDepth << "\n";
    std::cout << "Total nodes         : " << totalNodes << "\n";

    double ratio = (originalSizeBytes > 0) ? static_cast<double>(compressedSizeBytes) / originalSizeBytes : 0.0;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Compression ratio   : " << (ratio * 100.0) << " %\n";
    std::cout << "==================================\n";
}
