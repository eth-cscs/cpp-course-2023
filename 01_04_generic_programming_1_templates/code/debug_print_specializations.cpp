#include <iostream>
#include "demangle_helper.hpp"

template <typename T, typename U = int>
struct X {
    X() {
        std::cout << "Primary        : " << debug::print_type<X<T,U>, T, U>(", ") << "\n";
    }
};

template <typename T>
struct X<T, typename T::extra_type> {
    X() {
        std::cout << "Specialization : " << debug::print_type<X<T,typename T::extra_type>, T, typename T::extra_type>(", ") << "\n";
    }
};

struct A { using value_type = int; };
struct B { using extra_type = int; };
struct C { using extra_type = float; };
struct D { using extra_type = char; };

int main() {
    X<A> a;         // uses primary - A::value_type not a match
    X<B> b;         // uses specialization - B::extra_type = int
    X<C> c;         // uses primary - C::extra_type not int
    X<B,char> b1;   // uses primary - B::extra_type not char
    X<D,char> d;    // uses specialization, D::extra_type = char
    const char *ptr = "This is a test";
    std::cout << "ptr type is    : " << debug::print_type<decltype(ptr)>("") << std::endl;
    std::cout << "std::free is   : " << debug::print_type<decltype(std::free)>("") << std::endl;
    std::cout << "no params < >  : " << debug::print_type<>("") << std::endl;

}

