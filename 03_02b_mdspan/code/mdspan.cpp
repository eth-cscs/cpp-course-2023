#include <experimental/mdspan>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <span>

void extents_snippets() {
    auto ext1 = std::extents<int, std::dynamic_extent, 3, std::dynamic_extent, 4>{ 42, 43 };
    static_assert(decltype(ext1)::rank() == 4);
    static_assert(decltype(ext1)::rank_dynamic() == 2);
    static_assert(decltype(ext1)::static_extent(0) == std::dynamic_extent);
    assert(ext1.extent(0) == 42);
    static_assert(decltype(ext1)::static_extent(1) == 3);

    auto ext2 = std::extents<std::uint8_t, 3, 4>{};
    static_assert(decltype(ext2)::static_extent(0) == 3);
    static_assert(decltype(ext2)::static_extent(1) == 4);

    auto ext3 = std::extents{ 42, 44 };
    static_assert(decltype(ext3)::static_extent(42) == std::dynamic_extent);
    assert(ext3.extent(0) == 42);

    auto ext4 = std::dextents<int, 3>{ 42, 43, 44 };
}

void layout_strides() {
    std::vector<float> v(100);

    using strided_md_span = std::mdspan<float, std::dextents<std::size_t, 3>, std::layout_stride>;
    auto s = strided_md_span(v.data(), { std::dextents<std::size_t, 3>(2, 5, 10), std::array<std::size_t, 3>{ 5, 1, 10 } });
    
    s[1, 2, 3] = 42;
    assert((v[2 + 5 + 30] == s[1, 2, 3]));
}

int main() {
    extents_snippets();
    layout_strides();

    std::vector<float> v(100);

    using ext_t = std::extents<int, std::dynamic_extent, std::dynamic_extent>;
    auto v_ext = ext_t{ 10, 10 };
    auto v_span_explicit = std::mdspan<float, ext_t>{ v.data(), v_ext };

    auto v_span = std::mdspan(v.data(), std::extents{ 10, 10 });
    for (std::size_t i : std::ranges::iota_view(0, 9))
    {
        for (std::size_t j : std::ranges::iota_view(0, 9)) {
            v_span[i, j] = i * 10 + j;
        }
    }

    // std::default_accessor<int>

    std::cout << v[15] << std::endl;
}
