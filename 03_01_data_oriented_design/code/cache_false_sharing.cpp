#include <algorithm>
#include <chrono>
#include <iostream>
#include <new>
#include <numeric>
#include <span>
#include <thread>
#include <vector>

#ifdef _MSC_VER
    #define NOINLINE __declspec(noinline)
#else
    #define NOINLINE __attribute__((noinline))
#endif


std::vector<std::span<const int64_t>> partition_values(std::span<const int64_t> values, size_t nthreads) {
    std::vector<std::span<const int64_t>> partitions;

    const auto partition_size = (values.size() + nthreads - 1) / nthreads;
    for (size_t thread_idx = 0; thread_idx < nthreads; ++thread_idx) {
        const auto first = partition_size * thread_idx;
        const auto last = std::min(values.size(), partition_size * (thread_idx + 1));
        partitions.push_back(values.subspan(first, last - first));
    }

    return partitions;
}


NOINLINE void do_sum(volatile int64_t* out, std::span<const int64_t> values) {
    for (const auto& item : values) {
        const auto current = *out;
        *out = current + item;
    }
}


int64_t false_sharing(std::span<const int64_t> values, size_t nthreads) {
    const auto partitions = partition_values(values, nthreads);
    std::vector<int64_t> partition_sums(nthreads, 0);

    std::vector<std::thread> threads;

    for (size_t thread_idx = 0; thread_idx < nthreads; ++thread_idx) {
        threads.push_back(std::thread([&partition_sums, &partitions, thread_idx] {
            do_sum(&partition_sums[thread_idx], partitions[thread_idx]);
        }));
    }

    std::ranges::for_each(threads, [](auto& th) { th.join(); });
    return std::reduce(partition_sums.begin(), partition_sums.end(), 0LL);
}


int64_t no_false_sharing(std::span<const int64_t> values, size_t nthreads) {
    const auto partitions = partition_values(values, nthreads);
    std::vector<int64_t> partition_sums(nthreads, 0);

    std::vector<std::thread> threads;

    for (size_t thread_idx = 0; thread_idx < nthreads; ++thread_idx) {
        threads.push_back(std::thread([&partition_sums, &partitions, thread_idx] {
            int64_t local = 0;
            do_sum(&local, partitions[thread_idx]);
            partition_sums[thread_idx] = local;
        }));
    }

    std::ranges::for_each(threads, [](auto& th) { th.join(); });
    return std::reduce(partition_sums.begin(), partition_sums.end(), 0LL);
}


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    std::vector<int64_t> values(1'000'000'000, 1);
    const size_t max_nthreads = std::min(size_t(std::thread::hardware_concurrency()), size_t(64) / sizeof(int64_t));

    for (int nthreads = 1; nthreads <= max_nthreads; ++nthreads) {
        std::cout << "nthreads=" << nthreads << std::endl;
        {
            const auto start = high_resolution_clock::now();
            const auto result = no_false_sharing(values, nthreads);
            const auto end = high_resolution_clock::now();
            std::cout << "  no false sharing: "
                      << duration_cast<milliseconds>(end - start).count() << " ms"
                      << " (result=" << result << ")"
                      << std::endl;
        }

        {
            const auto start = high_resolution_clock::now();
            const auto result = false_sharing(values, nthreads);
            const auto end = high_resolution_clock::now();
            std::cout << "  false sharing:    "
                      << duration_cast<milliseconds>(end - start).count() << " ms"
                      << " (result=" << result << ")"
                      << std::endl;
        }
    }
}