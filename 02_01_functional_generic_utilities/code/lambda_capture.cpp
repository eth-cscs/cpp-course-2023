#include <iostream>
#include <utility>

template <typename>
struct check_type {} // Comment out this line to make the compiler print the types of the captures
;

void print(int&& x) { std::cout << "int&&, x = " << x << '\n'; }
void print(int const& x) { std::cout << "int const&, x = " << x << '\n'; }

int main() {
    int x{ 42 };
    int& y = x;

    // What are the types of decltype(x), decltype((x)), decltype(y), decltype((y)) below?
    // Not all lines are meant to without errors.

    // [x] { check_type<decltype(x)> c; ++x; print(std::move(x)); }();
    // [x] { check_type<decltype((x))> c; ++x; print(std::move(x)); }();
    // [&x] { check_type<decltype(x)> c; ++x; print(std::move(x)); }();
    // [&x] { check_type<decltype((x))> c; ++x; print(std::move(x)); }();
    // [x]() mutable { check_type<decltype(x)> c; ++x; print(std::move(x)); }();
    // [x]() mutable { check_type<decltype((x))> c; ++x; print(std::move(x)); }();
    // [&x]() mutable { check_type<decltype(x)> c; ++x; print(std::move(x)); }();
    // [&x]() mutable { check_type<decltype((x))> c; ++x; print(std::move(x)); }();

    // [y] { check_type<decltype(y)> c; ++y; print(std::move(y)); }();
    // [y] { check_type<decltype((y))> c; ++y; print(std::move(y)); }();
    // [&y] { check_type<decltype(y)> c; ++y; print(std::move(y)); }();
    // [&y] { check_type<decltype((y))> c; ++y; print(std::move(y)); }();
    // [y]() mutable { check_type<decltype(y)> c; ++y; print(std::move(y)); }();
    // [y]() mutable { check_type<decltype((y))> c; ++y; print(std::move(y)); }();
    // [&y]() mutable { check_type<decltype(y)> c; ++y; print(std::move(y)); }();
    // [&y]() mutable { check_type<decltype((y))> c; ++y; print(std::move(y)); }();
}
