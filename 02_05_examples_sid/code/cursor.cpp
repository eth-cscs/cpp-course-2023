#include <array>
#include <iostream>
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

void dump(Cursor auto c) {
    for (; !c.done(); c.next()) {
        std::cout << c.get() << std::endl;
    }
}

// ===

// file cursor

// ===

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



int main() {
    dump(a_very_concrete_cursor{});
    // dump(numbers_from(5));
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
    dump(take(10)(squared(numbers_from(2))));
}
