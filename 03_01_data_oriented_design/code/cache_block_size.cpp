#include <algorithm>
#include <chrono>
#include <immintrin.h>
#include <iostream>
#include <new>
#include <numeric>
#include <span>
#include <thread>
#include <vector>


std::vector<size_t> GetBlockSizes(size_t maxSize) {
    std::vector<size_t> blockSizes;
    for (size_t s = 128; s <= maxSize; s *= 2) {
        blockSizes.push_back(s);
        if (s != maxSize) {
            blockSizes.push_back(s * 5 / 4);
            blockSizes.push_back(s * 6 / 4);
            blockSizes.push_back(s * 7 / 4);
        }
    }
    return blockSizes;
}


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    using std::chrono::nanoseconds;

    constexpr size_t burstSize = 8;
    constexpr size_t maxBlockSize = 512 * 1048576;

    for (auto blockSize : GetBlockSizes(maxBlockSize)) {
        std::vector<int64_t> values(blockSize / sizeof(int64_t), 1);

        size_t result = 0;
        __m256i acc = _mm256_setzero_si256();
        const size_t reps = 10 * maxBlockSize / blockSize;

        const auto start = high_resolution_clock::now();
        for (size_t rep = 0; rep < reps; ++rep) {
            const auto step = 4 * sizeof(__m256i) / sizeof(int64_t);
            auto first = reinterpret_cast<const __m256i*>(values.data());
            const auto last = reinterpret_cast<const __m256i*>(values.data() + values.size() / step * step);
            result += sizeof(__m256i) * (last - first);
            while (first < last) {
                const auto v0 = _mm256_loadu_si256(first++);
                const auto v1 = _mm256_loadu_si256(first++);
                const auto v2 = _mm256_loadu_si256(first++);
                const auto v3 = _mm256_loadu_si256(first++);
                const auto tmp0 = _mm256_or_si256(acc, v0);
                const auto tmp1 = _mm256_or_si256(v1, v2);
                const auto tmp2 = _mm256_or_si256(tmp0, tmp1);
                acc = _mm256_or_si256(tmp2, v3);
            }
        }
        const auto end = high_resolution_clock::now();

        if (_mm256_extract_epi64(acc, 0) != 1) {
            throw std::logic_error("incorrect result");
        }

        const auto time = duration_cast<nanoseconds>(end - start);
        const auto bandwidth = result / float(time.count());

        std::cout << "  block size = " << blockSize << ":    "
                  << duration_cast<milliseconds>(time).count() << " ms, "
                  << bandwidth << " GB/s" << std::endl;
    }
}