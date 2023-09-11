#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <span>
#include <vector>
#include <immintrin.h>



std::mt19937_64 rne;


struct OffsetGenerator {
    OffsetGenerator(size_t rangeSize, size_t blockSize)
        : blockSize(blockSize),
          linearSize((rangeSize - blockSize + 1) / blockSize),
          rowSize(64),
          columnSize(linearSize / rowSize) {
        assert(rangeSize > blockSize);
    }

    size_t operator()() {
        const auto linearIdx = rowSize * columnIdx + rowIdx;
        columnIdx += 1;
        if (columnIdx >= columnSize) {
            rowIdx = (rowIdx + 1) % rowSize;
            columnIdx = 0;
        }
        return blockSize * linearIdx;
    }

    const size_t blockSize;
    const size_t linearSize;
    const size_t rowSize;
    const size_t columnSize;
    size_t rowIdx = 0;
    size_t columnIdx = 0;
};


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;
    using std::chrono::milliseconds;

    // 2 gigabytes to overflow L3 cache
    constexpr size_t dataVolume = 2ull * 1024 * 1024 * 1024;
    constexpr size_t numValues = dataVolume / sizeof(int64_t);
    constexpr size_t reps = 200;

    std::unique_ptr<int64_t, void (*)(void*)> data{
        static_cast<int64_t*>(std::aligned_alloc(8192, numValues * sizeof(int64_t))),
        [](void* ptr) { std::free(ptr); }
    };
    std::span<int64_t> values(data.get(), numValues);
    std::ranges::fill(values, 1);


    for (size_t blockSize = 4; blockSize <= 1048576; blockSize *= 2) {
        OffsetGenerator gen{ values.size(), blockSize };
        int64_t result = 0;
        __m256i s = _mm256_setzero_si256();

        const auto start = high_resolution_clock::now();
        for (int i = 0; i < 1048576 * reps / blockSize; ++i) {
            const auto offset = gen();
            const auto range = values.subspan(offset, blockSize);
            for (auto it = range.begin(); it < range.end(); it += 4) {
                const auto v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&*it));
                s = _mm256_xor_si256(s, v);
            };
            result += blockSize;
        }
        const auto end = high_resolution_clock::now();

        if (result != 1048576 * reps + (s[0] & ~int64_t(0))) {
            throw std::logic_error("incorrect result");
        }

        const auto time = duration_cast<nanoseconds>(end - start);
        const float bandwidth = result * sizeof(int64_t) / float(time.count());
        std::cout << "block size = " << blockSize << ":    "
                  << duration_cast<milliseconds>(time).count() << " ms, "
                  << bandwidth << " GB/s" << std::endl;
    }
}
