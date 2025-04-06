#ifndef ERROR_CALCULATOR_HPP
#define ERROR_CALCULATOR_HPP

#include <vector>
#include <cstdint>
using namespace std;

// Penomoran metode error
#define VARIANCE_METHOD 1
#define MAD_METHOD 2
#define MAX_DIFF_METHOD 3
#define ENTROPY_METHOD 4
#define SSIM_METHOD 5

namespace ErrorCalculator {
    typedef vector<uint8_t> Block;

    // Semua fungsi menerima dua blok (dengan lebar & tinggi sama), dan ukuran blok
    double calculateMAD(const Block& block, int width, int height) ;
    double calculateMaxDiff(const Block& block1, const Block& block2, int width, int height);
    double calculateVariance(const Block& block, int width, int height);
    double calculateEntropy(const Block& block, int width, int height);
    double calculateSSIM(const Block& block1, const Block& block2, int width, int height);

    // Fungsi dispatcher berdasarkan kode metode
    double calculateError(int methodId, const Block& block1, const Block& block2, int width, int height);
}

#endif
