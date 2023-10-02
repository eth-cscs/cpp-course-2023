#include <array>
#include <cstdint>
#include <type_traits>

// simple concrete example, models `md_iterable`
namespace some_library {
template <std::size_t N>
class some_md_iterable {
public:
    double* origin() const;
    std::array<std::size_t, N> strides() const;
};

template <std::size_t N>
double* mdi_get_origin(some_md_iterable<N> m) {
    return m.origin();
}

template <std::size_t N>
std::array<std::size_t, N> mdi_get_strides(some_md_iterable<N> m) {
    return m.strides();
}


} // namespace some_library


// // uses `md_iterable`
// void five_point_stencil(auto iterable){

//     auto strides = iterable.strides();

//     auto center = iterable.origin();
//     auto left = center - stride etc
// }


namespace md_iterable {

template <class T>
concept is_pointer = std::is_pointer_v<T>;

template <class T>
concept MD_Iterable = std::move_constructible<T> && requires(T const& mdi) {
    {
        mdi.origin()
        } -> is_pointer;
};
static_assert(MD_Iterable<some_library::some_md_iterable<1>>);

auto shift(auto& ptr, auto stride, auto offset) {
    return ptr + stride * offset;
}
template <class T>
concept MD_Pointer = std::move_constructible<T> && requires(T const& ptr) {
    { *ptr };
    { md_iterable::shift(ptr, 0, 0) };
};

namespace tests {
    static_assert(MD_Pointer<double*>);
} // namespace tests

template <class T>
auto get_origin(T&& mdi) {
    return mdi_get_origin(std::forward<T>(mdi));
}

template <class T>
auto get_strides(T&& mdi) {
    return mdi_get_strides(std::forward<T>(mdi));
}

// template<class Ptr, class Stride, class Offset>

} // namespace md_iterable

int main() {
    using foo = decltype(md_iterable::get_origin(some_library::some_md_iterable<3>{}));
}
