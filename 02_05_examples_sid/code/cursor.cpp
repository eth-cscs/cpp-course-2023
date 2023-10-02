#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

struct some_cursor {
    bool done() const;
    int const& get() const;
    void next();
};

class a_very_concrete_cursor {
    std::array<int, 5> data = { 1, 2, 3, 4, 5 };
    int i = 0;

public:
    bool done() const { return i >= data.size(); }
    int const& get() const { return data[i]; }
    void next() {
        ++i;
    }
};

// void dump(auto c) {
//     for (; !c.done(); c.next()) {
//         std::cout << c.get() << std::endl;
//     }
// }

// ===

template <class T>
concept ConstRef = std::is_lvalue_reference_v<T> && std::is_const_v<std::remove_reference_t<T>>;

template <class T>
concept Cursor = std::move_constructible<T> && requires(T& t, T const& tc) {
    {
        tc.get()
    }; // -> ConstRef;
    {
        tc.done()
        } -> std::convertible_to<bool>;
    { t.next() };
};

static_assert(Cursor<some_cursor>);

// void dump(Cursor auto c) {
//     for (; !c.done(); c.next()) {
//         std::cout << c.get() << std::endl;
//     }
// }

inline constexpr auto dump = [](Cursor auto c) {
    for (; !c.done(); c.next()) {
        std::cout << c.get() << std::endl;
    }
};

// ===

// file cursor

// ===

template <std::integral T>
struct range {
    T start_;
    T stop_;

    T const& get() const { return start_; }
    void next() { ++start_; };
    bool done() const { return start_ >= stop_; }
};


template <std::integral T>
struct numbers_from {
    T value_;

    T const& get() const { return value_; }
    void next() { ++value_; };
    bool done() const { return false; }
};

template <Cursor T>
struct take_impl {
    T wrapped_;
    int left_;

    auto get() const { return wrapped_.get(); }
    void next() {
        wrapped_.next();
        left_--;
    }
    bool done() const {
        return left_ <= 0;
    }
};

constexpr auto take = [](int n) {
    return [n](Cursor auto c) {
        return take_impl(std::move(c), n);
    };
};

// ===

// can we define CursorAlgorithm concept?

template <Cursor T>
struct square_impl {
    T cur_;

    auto get() const {
        return cur_.get() * cur_.get();
    }

    void next() {
        cur_.next();
    }

    bool done() const {
        return cur_.done();
    }
};


template <Cursor T, class Fun>
struct transform_impl {
    T cur_;
    Fun fun_;

    auto get() const {
        return fun_(cur_.get());
    }

    void next() {
        cur_.next();
    }

    bool done() const {
        return cur_.done();
    }
};

constexpr auto transform = [](auto fun) {
    return [fun = std::move(fun)](Cursor auto c) {
        return transform_impl(std::move(c), std::move(fun));
    };
};

auto squared = transform([](auto a) { return a * a; });


// ===

template <class F>
constexpr F&& compose(F&& f) {
    return std::forward<F>(f);
}

template <class F, class... Gs>
constexpr auto compose(F&& f, Gs&&... gs) {
    return [f = std::forward<F>(f), g = compose(std::forward<Gs>(gs)...)]<class... Args>(Args&&... args) { return f(g(args...)); };
}

// ===

namespace pipes {
template <class F, class G>
constexpr decltype(auto) operator|(F&& f, G&& g) {
    if constexpr (std::is_invocable_v<G&&, F&&>)
        return std::forward<G>(g)(std::forward<F>(f));
    else
        return compose(g, f);
}
} // namespace pipes

// ===

template <class T>
class any_cursor {
    struct iface {
        virtual bool done() const = 0;
        virtual void next() = 0;
        virtual T const& get() const = 0;
        virtual ~iface(){};
    };

    template <Cursor C>
    struct impl : iface {
        C cur_;
        impl(C c) : cur_{ std::move(c) } {}
        bool done() const {
            return cur_.done();
        }
        void next() {
            cur_.next();
        }
        T const& get() const {
            return cur_.get();
        }
    };

    std::unique_ptr<iface> impl_;

public:
    any_cursor(Cursor auto c) : impl_(new impl(std::move(c))) {}
    bool done() const {
        return impl_->done();
    }
    T const& get() const {
        return impl_->get();
    }
    void next() {
        return impl_->next();
    }
};

void my_complicated_algorithm(any_cursor<int> c) {
    dump(std::move(c));
}

// ===

namespace AnotherAPI {
template <class T>
struct NumbersFrom {
    T value_;

    T const& Get() const { return value_; }
    void Next() { ++value_; };
    bool Done() const { return false; }
};
} // namespace AnotherAPI

static_assert(!Cursor<AnotherAPI::NumbersFrom<int>>); // not a cursor :(

template <class C>
struct cursor_wrapper {
    C another_api_cur_;
    auto get() const {
        return another_api_cur_.Get();
    }
    void next() {
        another_api_cur_.Next();
    }
    bool done() const {
        return another_api_cur_.Done();
    }
};
template <class T>
using adopted_numbers_from = cursor_wrapper<AnotherAPI::NumbersFrom<T>>;

static_assert(Cursor<adopted_numbers_from<int>>);


int main() {
    dump(a_very_concrete_cursor{});
    std::cout << "===\n";
    dump(range(2, 5));
    std::cout << "===\n";
    dump(take_impl(numbers_from(7), 10));
    std::cout << "===\n";
    dump(take(10)(numbers_from(7)));
    std::cout << "===\n";
    dump(take(10)(square_impl(numbers_from(2))));
    std::cout << "===\n";
    dump(take(10)(transform_impl(numbers_from(2), [](auto a) { return a * a; })));
    std::cout << "===\n";
    dump(take(10)(transform([](auto a) { return a * a; })(numbers_from(2))));
    std::cout << "===\n";
    dump(squared(take(10)(squared(numbers_from(2)))));

    std::cout << "===\n";
    auto composed = compose(squared, take(10), squared);
    dump(composed(numbers_from(42)));
    std::cout << "===\n";


    using namespace pipes;
    auto take_10_and_dump = take(10) | dump;
    range(100, 120) | take_10_and_dump;


    // range(2, 5) | dump;

    // dump(numbers_from(42) | take(10));
    // dump(compose(take(10), numbers_from(42)));

    std::cout << "===\n";
    any_cursor<int> a = { range(10, 20) };
    my_complicated_algorithm(std::move(a));
}
