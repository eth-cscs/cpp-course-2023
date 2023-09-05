#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <span>
#include <thread>
#include <vector>



auto get_my_values(std::span<const int64_t> values, size_t my_idx, size_t nthreads) {
    const auto my_base = (values.size() + nthreads - 1) / nthreads;
    const auto my_start = my_base * my_idx;
    const auto my_end = std::min(values.size(), my_base * (my_idx + 1));
    const auto my_values = values.subspan(my_start, my_end - my_start);
    return my_values;
}


int64_t with_false_sharing(std::span<const int64_t> values) {
    constexpr size_t nthreads = 16;
    std::array<int64_t, nthreads> partial_sums;
    std::ranges::fill(partial_sums, 0);
    std::array<std::thread, nthreads> threads;
    size_t result_idx = 0;
    std::ranges::generate(threads, [&partial_sums, nthreads, values, &result_idx] {
        return std::thread([=, &partial_sums, my_idx = result_idx++] {
            const auto my_values = get_my_values(values, my_idx, nthreads);
            // NOTE: Writing 'partial_sum' once for each item.
            std::ranges::for_each(my_values, [&](auto item) { partial_sums[my_idx] += item; });
        });
    });
    std::ranges::for_each(threads, [](auto& th) { th.join(); });
    return std::reduce(partial_sums.begin(), partial_sums.end(), 0);
}


int64_t without_false_sharing(std::span<const int64_t> values) {
    constexpr size_t nthreads = 16;
    std::array<int64_t, nthreads> partial_sums;
    std::ranges::fill(partial_sums, 0.0f);
    std::array<std::thread, nthreads> threads;
    size_t result_idx = 0;
    std::ranges::generate(threads, [&partial_sums, nthreads, values, &result_idx] {
        return std::thread([=, &partial_sums, my_idx = result_idx++] {
            const auto my_values = get_my_values(values, my_idx, nthreads);
            int64_t my_partial_sum = 0;
            std::ranges::for_each(my_values, [&](auto item) { my_partial_sum += item; });
            // NOTE: Writing 'partial_sum' only once.
            partial_sums[my_idx] = my_partial_sum;
        });
    });
    std::ranges::for_each(threads, [](auto& th) { th.join(); });
    return std::reduce(partial_sums.begin(), partial_sums.end(), 0);
}


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;

    std::vector<int64_t> values(100'000'000, 1);

    {
        const auto start = high_resolution_clock::now();
        std::cout << without_false_sharing(values) << std::endl;
        const auto end = high_resolution_clock::now();
        std::cout << "without false sharing: " << nanoseconds(end - start) << std::endl;
    }

    {
        const auto start = high_resolution_clock::now();
        std::cout << with_false_sharing(values) << std::endl;
        const auto end = high_resolution_clock::now();
        std::cout << "with false sharing: " << nanoseconds(end - start) << std::endl;
    }
}