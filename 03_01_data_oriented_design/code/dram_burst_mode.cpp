#include <chrono>
#include <iostream>
#include <span>
#include <vector>


int64_t sum_all(std::span<int64_t> values) {
    int64_t sum = 0;
    for (size_t idx = 0; idx < (values.size() & ~7u); idx += 8) {
        // Pairwise sum of all elements in a block of 8.
        sum += ((values[idx + 0] + values[idx + 1])
                + (values[idx + 2] + values[idx + 3]))
               + ((values[idx + 4] + values[idx + 5])
                  + (values[idx + 6] + values[idx + 7]));
    }
    return sum;
}

int64_t sum_every_8th(std::span<int64_t> values) {
    int64_t sum = 0;
    for (size_t idx = 0; idx < (values.size() & ~7u); idx += 8) {
        // Pick first element out of a block of 8.
        sum += values[idx + 0];
    }
    return sum;
}


int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;
    using std::chrono::milliseconds;

    // 2 gigabytes to overflow L3 cache
    constexpr size_t data_volume = 2ull * 1024 * 1024 * 1024;
    constexpr size_t num_values = data_volume / sizeof(int64_t);
    constexpr size_t reps = 20;

    std::vector<int64_t> values(num_values, 1);

    const auto t1 = high_resolution_clock::now();
    for (int i = 0; i < reps; ++i) {
        const auto result = sum_all(values);
        if (result != num_values) {
            throw std::logic_error("incorect result");
        }
    }
    const auto t2 = high_resolution_clock::now();
    for (int i = 0; i < reps; ++i) {
        const auto result = sum_every_8th(values);
        if (result != num_values / 8) {
            throw std::logic_error("incorect result");
        }
    }
    const auto t3 = high_resolution_clock::now();

    const auto time_all = duration_cast<nanoseconds>(t2 - t1);
    const auto time_8th = duration_cast<nanoseconds>(t3 - t2);
    const float bandwidth_all = reps * data_volume / float(time_all.count());
    const float bandwidth_8th = reps * data_volume / float(time_8th.count()) / 8;


    std::cout << "sum all:       "
              << duration_cast<milliseconds>(time_all).count() << " ms, "
              << bandwidth_all << " GB/s" << std::endl;
    std::cout << "sum every 8th: "
              << duration_cast<milliseconds>(time_8th).count() << " ms, "
              << bandwidth_8th << " GB/s" << std::endl;
}