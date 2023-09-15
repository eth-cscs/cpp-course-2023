#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <immintrin.h>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <span>
#include <vector>



std::mt19937_64 rne;


struct OffsetGenerator {
    OffsetGenerator(size_t rangeSize, size_t rowSize, bool shuffle)
        : rowSize(rowSize),
          columnSize(rangeSize / rowSize) {
        rowShuffle.resize(rowSize);
        std::iota(rowShuffle.begin(), rowShuffle.end(), uint16_t(0));
        if (shuffle) {
            std::ranges::shuffle(rowShuffle, rne);
        }

        columnShuffle.resize(columnSize);
        std::iota(columnShuffle.begin(), columnShuffle.end(), uint32_t(0));
        std::ranges::shuffle(columnShuffle, rne);
    }

    size_t operator()() {
        const auto linearIdx = rowSize * columnShuffle[columnIdx] + rowShuffle[rowIdx];
        rowIdx += 1;
        if (rowIdx >= rowSize) {
            columnIdx = (columnIdx + 1) % columnSize;
            rowIdx = 0;
        }
        return linearIdx;
    }

    const size_t rowSize;
    const size_t columnSize;
    size_t rowIdx = 0;
    size_t columnIdx = 0;
    std::vector<uint16_t> rowShuffle;
    std::vector<uint32_t> columnShuffle;
};


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;
    using std::chrono::milliseconds;

    // 2 gigabytes to overflow L3 cache
    constexpr size_t dataVolume = 2ull * 1024 * 1024 * 1024;
    constexpr size_t numValues = dataVolume / sizeof(int64_t);
    constexpr size_t reps = 50;
    constexpr size_t burstSize = 8;

    std::unique_ptr<int64_t, void (*)(void*)> data{
        static_cast<int64_t*>(::operator new[](sizeof(int64_t) * numValues, std::align_val_t{ 65536 })),
        [](void* ptr) { ::operator delete[](ptr, std::align_val_t{ 65536 }); }
    };
    std::span<int64_t> values(data.get(), numValues);
    std::ranges::fill(values, 1);


    for (bool shuffle : { true, false }) {
        std::cout << "shuffling: " << std::boolalpha << shuffle << std::endl;
        for (size_t rowSize = 1; rowSize <= 65536; rowSize *= 2) {
            OffsetGenerator gen{ values.size() / burstSize, rowSize, shuffle };
            int64_t result = 0;
            std::array<int64_t*, 16> prefetch;
            std::ranges::generate(prefetch, [&]() { return values.data() + burstSize * gen(); });
            size_t prefetchIdx = 0;

            const auto start = high_resolution_clock::now();
            for (int i = 0; i < 1048576 * reps; ++i) {
                const auto currentPtr = prefetch[prefetchIdx];
                const auto prefetchedPtr = values.data() + burstSize * gen();
                _mm_prefetch(reinterpret_cast<const char*>(prefetchedPtr), _MM_HINT_T0);
                prefetch[prefetchIdx] = prefetchedPtr;
                prefetchIdx = (prefetchIdx + 1) % prefetch.size();
                result += *currentPtr;
            }
            const auto end = high_resolution_clock::now();

            if (result != 1048576 * reps) {
                throw std::logic_error("incorrect result");
            }

            const auto time = duration_cast<nanoseconds>(end - start);
            const float bandwidth = burstSize * result * sizeof(int64_t) / float(time.count());
            std::cout << "  block size = " << rowSize << ":    "
                      << duration_cast<milliseconds>(time).count() << " ms, "
                      << bandwidth << " GB/s" << std::endl;
        }
    }
}
