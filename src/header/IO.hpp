#ifndef IO_HPP
#define IO_HPP

#include <string>
using namespace std;

struct InputParams {
    string inputImagePath;
    int errorMethod;
    double threshold;
    int minBlockSize;
    double targetCompression;
    string outputImagePath;
    string outputGifPath; 
};

struct OutputStats {
    double executionTime;
    size_t originalSize;
    size_t compressedSize;
    double compressionRatio;
    int maxDepth;
    int totalNodes;
};

InputParams getUserInput();
void printOutputStats(const OutputStats& stats);
void printImageSavedMessage(const string& path);
void printGifSavedMessage(const string& path);

unsigned char* load_image(const char* filename, int& width, int& height, int& channels);

#endif 
