#ifndef ERROR_CALCULATOR_HPP
#define ERROR_CALCULATOR_HPP

#include <vector>
#include <cstdint>
using namespace std;

// Penomoran metode error
#define MSE_METHOD 1
#define MAE_METHOD 2
#define MAX_DIFF_METHOD 3
#define VARIANCE_METHOD 4
#define ENTROPY_METHOD 5
#define SSIM_METHOD 6

namespace ErrorCalculator {
    typedef vector<uint8_t> Block;

    // Semua fungsi menerima dua blok (dengan lebar & tinggi sama), dan ukuran blok
    double calculateMSE(const Block& block1, const Block& block2, int width, int height);
    double calculateMAE(const Block& block1, const Block& block2, int width, int height);
    double calculateMaxDiff(const Block& block1, const Block& block2, int width, int height);
    double calculateVariance(const Block& block, int width, int height);
    double calculateEntropy(const Block& block, int width, int height);
    double calculateSSIM(const Block& block1, const Block& block2, int width, int height);

    // Fungsi dispatcher berdasarkan kode metode
    double calculateError(int methodId, const Block& block1, const Block& block2, int width, int height);
}

#endif
