#include <concepts>
#include <iostream>
#include <type_traits>

#if __cpp_deduction_guides < 201907L
    #define CURSOR_NO_CTAD_AGGREGATES
#endif

// concept
namespace cursor {
// default done
auto cursor_done(...) { return false; }

// cursor fallback
auto cursor_done(auto const& cur) -> decltype(cur.done()) { return cur.done(); }
auto cursor_next(auto& cur) -> decltype(cur.next()) { cur.next(); }
auto cursor_get(auto const& cur) -> decltype(cur.get()) { return cur.get(); }

constexpr inline auto done = [](auto const& cur) -> decltype(cursor_done(cur)) {
    return cursor_done(cur);
};
constexpr inline auto next = [](auto& cur) -> decltype(cursor_next(cur)) {
    cursor_next(cur);
};
constexpr inline auto get = [](auto const& cur) -> decltype(cursor_get(cur)) {
    return cursor_get(cur);
};

template <class T>
concept Cursor =
    std::move_constructible<T> && requires(T& cur, T const& const_cur) {
    {
        done(const_cur)
        } -> std::convertible_to<bool>;
    get(const_cur);
    next(cur);
};

} // namespace cursor

// cursor and cursor algorithm implementations
namespace cursor_library {
using cursor::Cursor;

template <std::integral T>
struct numbers_from {
    T value_;

#ifdef CURSOR_NO_CTAD_AGGREGATES
    numbers_from(T value) : value_(value) {}
#endif

    friend T cursor_get(numbers_from const& cur) { return cur.value_; }
    friend void cursor_next(numbers_from& cur) { ++cur.value_; }
};
static_assert(Cursor<numbers_from<int>>);

template <Cursor C>
struct take_impl_ {
    C cur_;
    int left_;

#ifdef CURSOR_NO_CTAD_AGGREGATES
    take_impl_(C cur, int left) : cur_{ cur }, left_{ left } {}
#endif

    friend auto cursor_get(take_impl_ const& cur) -> decltype(auto) {
        return cursor::get(cur.cur_);
    }
    friend void cursor_next(take_impl_& cur) {
        --cur.left_;
        cursor::next(cur.cur_);
    }
    friend auto cursor_done(take_impl_ const& cur) -> decltype(auto) {
        return cursor::done(cur.cur_) || cur.left_ <= 0;
    }
};

auto take(int n) {
    return [n](Cursor auto cur) { return take_impl_(cur, n); };
}

template <class Fun, Cursor C>
struct transform_impl_ {
    Fun fun_;
    C cur_;

#ifdef CURSOR_NO_CTAD_AGGREGATES
    transform_impl_(Fun fun, C cur) : fun_{ std::move(fun) }, cur_{ std::move(cur) } {}
#endif

    friend auto cursor_get(transform_impl_ const& transf_) -> decltype(auto) {
        return transf_.fun_(cursor::get(transf_.cur_));
    }
    friend void cursor_next(transform_impl_& transf_) {
        cursor::next(transf_.cur_);
    }
    friend auto cursor_done(transform_impl_ const& transf_) -> decltype(auto) {
        return cursor::done(transf_.cur_);
    }
};

auto transform(auto f) {
    return [f = std::move(f)](Cursor auto cur) {
        return transform_impl_(std::move(f), std::move(cur));
    };
}

void dump(Cursor auto cur) {
    for (; !cursor::done(cur); cursor::next(cur)) {
        std::cout << cursor::get(cur) << std::endl;
    }
}

} // namespace cursor_library

int main() {
    using namespace cursor_library;

    dump(transform([](auto a) { return a * a; })(take(5)(numbers_from(5))));
}
