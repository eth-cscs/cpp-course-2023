#include <utility>
#include <iostream>

namespace ns {

    struct swap1 {

        swap1(int i) : data{i} {}

        friend void swap(swap1& a, swap1& b) {
            b.data = std::exchange(a.data, b.data);
        }

        int data;
    };

    struct swap2 {

        swap2(int i) : data{i} {}

        swap2(swap2 const &) = delete;

        friend void swap(swap2& a, swap2& b) {
            b.data = std::exchange(a.data, b.data);
        }

        int data;
    };

    struct swap3 {

        swap3(int i) : data{i} {}

        swap3(swap3&&) = default;
        swap3& operator=(swap3&&) = default;

        int data;
    };

}

template<typename T>
void print(T const& a, T const& b) {
    std::cout << a.data << " " << b.data << "\n";
}

int main() {

    ns::swap1 a{1}, b{2};
    print(a, b);
    swap(a, b);               // ok, found by ADL
    print(a, b);
    std::swap(a, b);          // ok, dispatch to friend function from within std::swap
    print(a, b);
    {                         // always ok
        using namespace std;
        swap(a, b);
    }
    print(a, b);

    ns::swap2 c{3}, d{4};
    print(c, d);
    swap(c, d);               // ok, found by ADL
    print(c, d);
    //std::swap(c, d);        // error:  not move constructible
    //print(c, d);
    {                         // always ok
        using namespace std;
        swap(c, d);
    }
    print(c, d);

    ns::swap3 e{5}, f{6};
    //swap(e, f);             // error, no swap function found
    //print(e, f);
    std::swap(e, f);          // ok, swap3 is move constructible and move assignable
    print(e, f);
    {                         // always ok
        using namespace std;
        swap(e, f);
    }
    print(e, f);

    return 0;
}
