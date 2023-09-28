#include <functional>
#include <iostream>
#include <string>
#include <utility>

template <typename F, typename TupleLike, std::size_t... Is>
decltype(auto) myapply_helper(/* what goes here? */) {
    return /* what goes here? */;
}

template <typename F, typename TupleLike>
decltype(auto) myapply(F&& f, TupleLike&& t) {
    return myapply_helper(
        std::make_index_sequence<std::tuple_size_v<TupleLike>>{},
        std::forward<F>(f), std::forward<TupleLike>(t));
}

struct myclass {
    void f(int x) const {
        std::cout << "myclass::f: (" << x << ")\n";
    }
};

int main() {
    // Implement myapply_helper correctly to get the following to compile
    // myapply([] { std::cout << "first tuple-like: ()\n"; }, std::tuple<>{});
    // myapply([](int x,
    //            double y) { std::cout << "second tuple-like: (" << x << ", " << y << ")\n"; },
    //         std::pair<int, double>{ 42, 3.14 });
    // myapply(
    //     [](std::string x, std::string y, std::string z) {
    //         std::cout << "second tuple-like: (" << x << ", " << y << "," << z << ")\n";
    //     },
    //     std::array<std::string, 3>{ "hello", "c++", "course" });

    // Extra: make the following compile as well
    // myapply(&myclass::f, std::tuple(myclass{}, 42));
}
