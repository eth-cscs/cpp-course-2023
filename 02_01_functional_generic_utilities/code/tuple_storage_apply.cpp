#include <functional>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

template <typename F, typename... Ts>
struct task {
    std::decay_t<F> f;
    std::tuple<std::decay_t<Ts>...> t;

    void operator()() {
        std::apply([&](auto&... ts) {
            f(ts...);
        }, t);
        // or if there are no additional arguments to pass to f simply:
        // std::apply(f, t);
    }
};

template <typename F, typename... Ts>
auto make_task(F&& f, Ts&&... ts) {
    return task<F, Ts...>{
        std::forward<F>(f),
        std::tuple<std::decay_t<Ts>...>(std::forward<Ts>(ts)...)
    };
}

void fill(int* array, int n, int x) {
    for (int i = 0; i < n; ++i) {
        array[i] = x;
    }
}

int main() {
    int n = 300;
    std::vector<int> v(n);
    int x = 42;

    auto t = make_task(fill, v.data(), v.size(), x);

    {
        // Initialize thread pool etc. to run task
        std::thread thr(t);
        thr.join();
    }
}
