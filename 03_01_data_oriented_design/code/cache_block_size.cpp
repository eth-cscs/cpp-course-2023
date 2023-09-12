#include <algorithm>
#include <chrono>
#include <iostream>
#include <new>
#include <numeric>
#include <span>
#include <thread>
#include <vector>


std::vector<size_t> GetBlockSizes(size_t maxSize) {
    std::vector<size_t> blockSizes;
    for (size_t s = 64; s <= maxSize; s *= 2) {
        blockSizes.push_back(s);
        if (s != maxSize) {
            blockSizes.push_back(s * 3 / 2);
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
        const size_t reps = 10 * maxBlockSize / blockSize;

        const auto start = high_resolution_clock::now();
        for (size_t rep = 0; rep < reps; ++rep) {
            for (size_t i = 0; i < values.size() / burstSize; ++i) {
                result += values[i * burstSize];
            }
        }
        const auto end = high_resolution_clock::now();

        if (result != reps * (values.size() / burstSize)) {
            throw std::logic_error("incorrect result");
        }

        const auto time = duration_cast<nanoseconds>(end - start);
        const auto bandwidth = burstSize * result * sizeof(int64_t) / float(time.count());

        std::cout << "  block size = " << blockSize << ":    "
                  << duration_cast<milliseconds>(time).count() << " ms, "
                  << bandwidth << " GB/s" << std::endl;
    }
}