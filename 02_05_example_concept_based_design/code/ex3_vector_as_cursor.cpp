#include "cursor.hpp"

#include <cassert>
#include <vector>

int main() {
    using namespace cursor::pipes;
    using namespace cursor_library;

    auto v = std::vector{ 1, 2, 3, 4, 5 };
    v | dump; // question: why does this compile?
    // prints 1, 2, 3, 4, 5
}
