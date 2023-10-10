#include "cursor.hpp"

int main() {
    using namespace cursor_library;

    // dump(transform([](auto a) { return a * a; })(take(5)(numbers_from(5))));

    using namespace cursor::pipes;
    numbers_from(5) | take(5) | dump;
    constexpr auto take_and_dump = take(5) | dump;
    numbers_from(-5) | take_and_dump;
    auto n = numbers_from(0);
    // n | take(5);
    // dump(take(5)(n));
    // _and_dump;
    n | take_and_dump;
    n | squared | take_and_dump;
    constexpr auto square_and_dump = squared | dump;
    // square_and_dump(n);

    auto algo = squared | take(5) | squared;
    numbers_from(2) | algo | dump;

    constexpr auto dump_any_int_cursor = [](cursor::any_cursor<int> cur) {
        for (; !cursor::done(cur); cursor::next(cur)) {
            std::cout << cursor::get(cur) << std::endl;
        }
    };

    auto any_c = cursor::any_cursor<int>(numbers_from(42) | take(2));
    dump_any_int_cursor(std::move(any_c));
}
