#include <array>
#include <cstdint>
#include <type_traits>

// simple concrete example, models `md_iterable`
template <std::size_t N>
class some_md_iterable {
public:
    double* origin() const;
    std::array<std::size_t, N> strides() const;
};



// // uses `md_iterable`
// void five_point_stencil(auto iterable){

//     auto strides = iterable.strides();

//     auto center = iterable.origin();
//     auto left = center - stride etc
// }


namespace md_iterable {

template <class T>
concept is_array = std::is_array_v<T>;

template <class T>
concept is_pointer = std::is_pointer_v<T>;

template <class T>
concept MD_Iterable = std::move_constructible<T> && requires(T const& mdi) {
    mdi.origin()->is_pointer;
    // {
    //     mdi.strides()
    //     } -> is_array;
};

static_assert(MD_Iterable<some_md_iterable<1>>);


template <class T>
auto get_origin(T&& mdi) {
    return mdi_get_origin(std::forward<T>(mdi));
}

template <class T>
auto get_strides(T&& mdi) {
    return mdi_get_strides(std::forward<T>(mdi));
}
} // namespace md_iterable

int main() {}
