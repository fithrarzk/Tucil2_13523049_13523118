#include "header/ErrorCalculator.hpp"
#include <cmath>
#include <map>
#include <numeric>

namespace ErrorCalculator {
    double calculateMAD(const Block& block, int width, int height) {
        int channels = 3;
        int pixelCount = width * height;
        double mean[3] = {0.0, 0.0, 0.0};

        // Hitung rata-rata untuk tiap channel
        for (int i = 0; i < block.size(); i += channels) {
            for (int c = 0; c < channels; ++c) {
                mean[c] += block[i + c];
            }
        }
        for (int c = 0; c < channels; ++c) {
            mean[c] /= pixelCount;
        }

    // Hitung MAD: rata-rata nilai absolut deviasi dari mean
    double mad[3] = {0.0, 0.0, 0.0};
    for (int i = 0; i < block.size(); i += channels) {
        for (int c = 0; c < channels; ++c) {
            mad[c] += std::abs(static_cast<double>(block[i + c]) - mean[c]);
        }
    }
    for (int c = 0; c < channels; ++c) {
        mad[c] /= pixelCount;
    }

    return (mad[0] + mad[1] + mad[2]) / 3.0;
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
        const std::vector<uint8_t>& data = block;
        int pixelCount = width * height;

        double mean[3] = {0.0f, 0.0f, 0.0f};

        for (int i = 0; i < data.size(); i += 3) {
            mean[0] += data[i];     // R
            mean[1] += data[i + 1]; // G
            mean[2] += data[i + 2]; // B
        }
        mean[0] /= pixelCount;
        mean[1] /= pixelCount;
        mean[2] /= pixelCount;

        double variance[3] = {0.0f, 0.0f, 0.0f};
        for (int i = 0; i < data.size(); i += 3) {
            variance[0] += (data[i]     - mean[0]) * (data[i]     - mean[0]);
            variance[1] += (data[i + 1] - mean[1]) * (data[i + 1] - mean[1]);
            variance[2] += (data[i + 2] - mean[2]) * (data[i + 2] - mean[2]);
        }
        variance[0] /= pixelCount;
        variance[1] /= pixelCount;
        variance[2] /= pixelCount;

        return (variance[0] + variance[1] + variance[2]) / 3.0f;
    }


    double calculateEntropy(const Block& block, int width, int height) {
        int channels = 3;
        int totalPixels = width * height;
        double entropy = 0.0;

        for (int c = 0; c < channels; ++c) {
            std::map<uint8_t, int> freq;
            for (int i = c; i < block.size(); i += channels) {
                freq[block[i]]++;
            }

            double channelEntropy = 0.0;
            for (const auto& [val, count] : freq) {
                double p = static_cast<double>(count) / totalPixels;
                channelEntropy -= p * std::log2(p);
            }

            entropy += channelEntropy;
        }

        return entropy / channels;
    }

    double calculateSSIM(const Block& block1, const Block& block2, int width, int height) {
        const double C1 = 6.5025, C2 = 58.5225;
        int channels = 3;
        int totalPixels = width * height;

        double ssim_total = 0.0;

        for (int c = 0; c < channels; ++c) {
            double mu1 = 0.0, mu2 = 0.0;
            for (int i = c; i < block1.size(); i += channels) {
                mu1 += block1[i];
                mu2 += block2[i];
            }
            mu1 /= totalPixels;
            mu2 /= totalPixels;

            double sigma1_sq = 0.0, sigma2_sq = 0.0, sigma12 = 0.0;
            for (int i = c; i < block1.size(); i += channels) {
                double a = block1[i] - mu1;
                double b = block2[i] - mu2;
                sigma1_sq += a * a;
                sigma2_sq += b * b;
                sigma12 += a * b;
            }

            sigma1_sq /= totalPixels;
            sigma2_sq /= totalPixels;
            sigma12 /= totalPixels;

            double ssim = ((2 * mu1 * mu2 + C1) * (2 * sigma12 + C2)) /
                        ((mu1 * mu1 + mu2 * mu2 + C1) * (sigma1_sq + sigma2_sq + C2));

            ssim_total += ssim;
        }

        return ssim_total / channels;
    }

    double calculateError(int methodId, const Block& block1, const Block& block2, int width, int height) {
        switch (methodId) {
            case MAD_METHOD:
                return calculateMAD(block1, width, height);
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
