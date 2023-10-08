#include <experimental/mdspan>

#include <cstddef>
#include <ranges>

int main() {
    std::vector<float> a(100);
    auto a_span = std::mdspan(a.data(), std::extents{ 10, 10 });

    for (std::size_t i : std::ranges::iota_view(0, 9))
    {
        for (std::size_t j : std::ranges::iota_view(0, 9)) {
            a_span[i, j] = i * 10 + j;
        }
    }
}
