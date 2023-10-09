#include "cursor.hpp"

constexpr inline auto filter = [](/*TODO*/) { /*TODO*/ };

int main() {
    using namespace cursor::pipes;
    using namespace cursor_library;

    auto odd_predicate = [](auto a) { return a % 2 == 1; };

    numbers_from(2) | filter(odd_predicate) | take(5) | dump;
    // prints "3, 5, 7, 9, 11, "
}
