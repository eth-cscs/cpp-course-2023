#include <concepts>
#include <iostream>
#include <memory>
#include <type_traits>

#if __cpp_deduction_guides < 201907L
    #define CURSOR_NO_CTAD_AGGREGATES
#endif

// concept
namespace cursor {
// default done
auto inline cursor_done(...) { return false; }

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

namespace pipes {
    template <class F, class G>
    constexpr auto operator|(F&& f, G&& g) -> decltype(auto) {
        if constexpr (std::is_invocable_v<G&&, F&&>) {
            return std::forward<G>(g)(std::forward<F>(f));
        }
        else {
            return [g = std::forward<G>(g), f = std::forward<F>(f)](auto&&... args) {
                return g(f(std::forward<decltype(args)...>(args...)));
            };
        }
    }
}; // namespace pipes

template <class T>
class any_cursor {
    struct iface {
        virtual ~iface(){};
        virtual T get() const = 0;
        virtual bool done() const = 0;
        virtual void next() = 0;
    };

    template <Cursor C>
    struct impl : iface {
        C cur_;
        impl(C cur) : cur_(std::move(cur)) {}
        T get() const {
            return cursor::get(cur_);
        }
        bool done() const {
            return cursor::done(cur_);
        }
        void next() {
            cursor::next(cur_);
        }
    };

    std::unique_ptr<iface> impl_;

public:
    template <class C>
    any_cursor(C cur) : impl_{ new impl<C>(std::move(cur)) } {}

    friend auto cursor_done(any_cursor const& cur) -> decltype(auto) { return cur.impl_->done(); }
    friend auto cursor_next(any_cursor& cur) -> decltype(auto) {
        cur.impl_->next();
    }
    friend auto cursor_get(any_cursor const& cur) -> decltype(auto) {
        return cur.impl_->get();
    }
};

namespace impl_ {
    struct sentinel {};

    template <Cursor C>
    struct iter {
        C& cur_;

        decltype(auto) operator*() const {
            return cursor::get(cur_);
        }
        void operator++() {
            cursor::next(cur_);
        }
        bool operator!=(sentinel) const {
            return !cursor::done(cur_);
        }
    };
} // namespace impl_

template <Cursor C>
impl_::iter<C> begin(C& cur) { return { cur }; }
template <Cursor C>
impl_::sentinel end(C const& cur) { return {}; }

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

    friend T const& cursor_get(numbers_from const& cur) { return cur.value_; }
    friend void cursor_next(numbers_from& cur) { ++cur.value_; }
};
static_assert(Cursor<numbers_from<int>>);

template <Cursor C>
struct take_impl_ {
    C cur_;
    int left_;

#ifdef CURSOR_NO_CTAD_AGGREGATES
    template <Cursor T>
    take_impl_(T&& cur, int left) : cur_{ std::forward<T>(cur) }, left_{ left } {}
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

constexpr inline auto take = [](int n) {
    return [n]<Cursor C>(C&& cur) { return take_impl_<C>(std::forward<C>(cur), n); };
};

template <class Fun, Cursor C>
struct transform_impl_ {
    Fun fun_;
    C cur_;

#ifdef CURSOR_NO_CTAD_AGGREGATES
    template <Cursor T>
    transform_impl_(Fun fun, T&& cur) : fun_{ std::move(fun) }, cur_{ std::forward<T>(cur) } {}
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

constexpr inline auto transform = [](auto f) {
    return [f = std::move(f)]<Cursor C>(C&& cur) {
        return transform_impl_<decltype(f), C>(std::move(f), std::forward<C>(cur));
    };
};

constexpr inline auto squared = transform([](auto a) { return a * a; });

constexpr inline auto dump = [](Cursor auto cur) {
    for (auto v : cur) {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
};

using cursor::begin;
using cursor::end;
} // namespace cursor_library
