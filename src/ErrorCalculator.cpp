#include "header/ErrorCalculator.hpp"
#include <cmath>
#include <map>
#include <numeric>

namespace ErrorCalculator {
    double calculateMSE(const Block& block1, const Block& block2, int width, int height) {
        double mse = 0.0;
        int size = width * height * 3;
        for (int i = 0; i < size; ++i) {
            double diff = static_cast<double>(block1[i]) - static_cast<double>(block2[i]);
            mse += diff * diff;
        }
        return mse / size;
    }

    double calculateMAE(const Block& block1, const Block& block2, int width, int height) {
        double mae = 0.0;
        int size = width * height * 3;
        for (int i = 0; i < size; ++i) {
            mae += std::abs(static_cast<double>(block1[i]) - static_cast<double>(block2[i]));
        }
        return mae / size;
    }

    double calculateMaxDiff(const Block& block1, const Block& block2, int width, int height) {
        double maxDiff = 0.0;
        int size = width * height * 3;
        for (int i = 0; i < size; ++i) {
            double diff = std::abs(static_cast<double>(block1[i]) - static_cast<double>(block2[i]));
            if (diff > maxDiff) maxDiff = diff;
        }
        return maxDiff;
    }

    double calculateVariance(const Block& block, int width, int height) {
        double sum = 0.0, sumSq = 0.0;
        int size = width * height * 3;
        for (int i = 0; i < size; ++i) {
            sum += block[i];
            sumSq += block[i] * block[i];
        }
        double mean = sum / size;
        return (sumSq / size) - (mean * mean);
    }

    double calculateEntropy(const Block& block, int width, int height) {
        std::map<uint8_t, int> freq;
        for (uint8_t val : block) {
            freq[val]++;
        }
        double entropy = 0.0;
        int total = block.size();
        for (const auto& [value, count] : freq) {
            double p = static_cast<double>(count) / total;
            entropy -= p * std::log2(p);
        }
        return entropy;
    }

    double calculateSSIM(const Block& block1, const Block& block2, int width, int height) {
        // versi sederhana SSIM
        const double C1 = 6.5025, C2 = 58.5225;
        int size = width * height * 3;

        double mu1 = std::accumulate(block1.begin(), block1.end(), 0.0) / size;
        double mu2 = std::accumulate(block2.begin(), block2.end(), 0.0) / size;

        double sigma1_sq = 0, sigma2_sq = 0, sigma12 = 0;
        for (int i = 0; i < size; ++i) {
            double a = static_cast<double>(block1[i]) - mu1;
            double b = static_cast<double>(block2[i]) - mu2;
            sigma1_sq += a * a;
            sigma2_sq += b * b;
            sigma12 += a * b;
        }
        sigma1_sq /= size;
        sigma2_sq /= size;
        sigma12 /= size;

        return ((2 * mu1 * mu2 + C1) * (2 * sigma12 + C2)) /
               ((mu1 * mu1 + mu2 * mu2 + C1) * (sigma1_sq + sigma2_sq + C2));
    }

    double calculateError(int methodId, const Block& block1, const Block& block2, int width, int height) {
        switch (methodId) {
            case MSE_METHOD:
                return calculateMSE(block1, block2, width, height);
            case MAE_METHOD:
                return calculateMAE(block1, block2, width, height);
            case MAX_DIFF_METHOD:
                return calculateMaxDiff(block1, block2, width, height);
            case VARIANCE_METHOD:
                return calculateVariance(block1, width, height);
            case ENTROPY_METHOD:
                return calculateEntropy(block1, width, height);
            case SSIM_METHOD:
                return 1.0 - calculateSSIM(block1, block2, width, height); // SSIM tinggi = bagus, jadi dibalik
            default:
                return 0.0;
        }
    }

}
