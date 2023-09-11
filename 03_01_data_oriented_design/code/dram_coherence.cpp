#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <span>
#include <vector>


std::mt19937_64 rne;


template <class IndexFunc>
int64_t Sum(std::span<int64_t> values, IndexFunc indices, size_t count) {
    int64_t sum = 0;
    for (size_t i = 0; i < count; ++i) {
        const auto idx = indices();
        sum += values[idx];
    }
    return sum;
}


struct LinearIndices {
    size_t idx = 0;
    size_t operator()() {
        return 8 * idx++;
    }
};


struct ClusteredIndices {
    ClusteredIndices(size_t rangeSize, size_t clusterSize)
        : clusterSize(clusterSize),
          offsetTable(clusterSize - 1),
          clusterRng(0, (rangeSize / 8 - clusterSize + 1) / clusterSize) {
        std::iota(offsetTable.begin(), offsetTable.end(), uint16_t(0));
        std::ranges::shuffle(offsetTable, rne);
    }

    size_t operator()() {
        if (offsetIdx == clusterSize) {
            offsetIdx = 0;
            // clusterIdx = clusterRng(rne);
            clusterIdx++;
        }
        const auto offset = offsetTable[offsetIdx];
        ++offsetIdx;
        return 8 * (clusterIdx * clusterSize + offset);
    }

    size_t clusterSize;
    size_t offsetIdx = 0;
    size_t clusterIdx = 0;
    std::vector<uint16_t> offsetTable;
    std::uniform_int_distribution<size_t> clusterRng;
};


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;
    using std::chrono::milliseconds;

    // 2 gigabytes to overflow L3 cache
    constexpr size_t dataVolume = 2ull * 1024 * 1024 * 1024;
    constexpr size_t numValues = dataVolume / sizeof(int64_t);
    constexpr size_t reps = 1;
    constexpr size_t dramPageSize = 16384;
    constexpr size_t clusterSize = 8;

    std::unique_ptr<int64_t, void (*)(void*)> data{
        static_cast<int64_t*>(std::aligned_alloc(dramPageSize, numValues * sizeof(int64_t))),
        [](void* ptr) { std::free(ptr); }
    };
    std::span<int64_t> values(data.get(), numValues);
    std::ranges::fill(values, 1);


    const auto t1 = high_resolution_clock::now();
    for (int i = 0; i < reps; ++i) {
        const auto result = Sum(values, LinearIndices{}, values.size() / 8);
        if (result != numValues / 8) {
            throw std::logic_error("incorrect result");
        }
    }
    const auto t2 = high_resolution_clock::now();
    for (int i = 0; i < reps; ++i) {
        const auto result = Sum(values, ClusteredIndices{ values.size(), clusterSize }, values.size() / 8);
        if (result != numValues / 8) {
            throw std::logic_error("incorrect result");
        }
    }
    const auto t3 = high_resolution_clock::now();
    for (int i = 0; i < reps; ++i) {
        const auto result = Sum(values, ClusteredIndices{ values.size(), 65536 }, values.size() / 8);
        if (result != numValues / 8) {
            throw std::logic_error("incorrect result");
        }
    }
    const auto t4 = high_resolution_clock::now();

    const auto time_linear = duration_cast<nanoseconds>(t2 - t1);
    const auto time_cluster = duration_cast<nanoseconds>(t3 - t2);
    const auto time_shuffle = duration_cast<nanoseconds>(t4 - t3);
    const float bandwidth_linear = reps * dataVolume / float(time_linear.count());
    const float bandwidth_cluster = reps * dataVolume / float(time_cluster.count());
    const float bandwidth_shuffle = reps * dataVolume / float(time_shuffle.count());


    std::cout << "linear:    "
              << duration_cast<milliseconds>(time_linear).count() << " ms, "
              << bandwidth_linear << " GB/s" << std::endl;
    std::cout << "clustered: "
              << duration_cast<milliseconds>(time_cluster).count() << " ms, "
              << bandwidth_cluster << " GB/s" << std::endl;
    std::cout << "shuffled:  "
              << duration_cast<milliseconds>(time_shuffle).count() << " ms, "
              << bandwidth_shuffle << " GB/s" << std::endl;
}



// // Linear indices
// std::vector<uint32_t> indices(values.size() / 8);
// std::iota(indices.begin(), indices.end(), 0);
// std::ranges::for_each(indices, [](auto& idx) { idx *= 8; });

// // Globally shuffled indices
// auto shuffled_indices = indices;
// std::ranges::shuffle(shuffled_indices, rne);

// // Cluster shuffled indices
// auto cluster_shuffled_indices = indices;

// for (auto it = cluster_shuffled_indices.begin();
//      it < cluster_shuffled_indices.end() - cluster_size;
//      it += cluster_size) {
//     auto cluster = std::span{ it, it + cluster_size };
//     std::ranges::shuffle(cluster, rne);
// }