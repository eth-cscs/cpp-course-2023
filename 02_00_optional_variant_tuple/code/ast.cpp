#include <iostream>
#include <memory>
#include <variant>

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct lit {
    int x;
};
struct add;
struct mul;

template <typename... Ts>
using up = std::unique_ptr<Ts...>;

template <typename T, typename U1, typename U2>
auto mkop(U1&& u1, U2&& u2) {
    return std::make_unique<T>(std::forward<U1>(u1), std::forward<U2>(u2));
}

using ast = std::variant<lit, up<add>, up<mul>>;

struct add {
    ast x, y;
};
struct mul {
    ast x, y;
};

int eval(ast const& a) {
    return std::visit(
        overloaded(
            [](lit const& l) { return l.x; },
            [](up<add> const& a) { return eval(a->x) + eval(a->y); },
            [](up<mul> const& m) { return eval(m->x) * eval(m->y); }),
        a);
}

int main() {
    ast a{ mkop<add>(lit{ 3 }, mkop<mul>(lit{ 5 }, lit{ 3 })) };
    std::cout << "3 + (5 * 3) is " << eval(a) << '\n';
}
