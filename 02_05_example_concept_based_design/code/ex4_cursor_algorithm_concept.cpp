#include "cursor.hpp"

#include <cassert>
#include <type_traits>
#include <vector>

// write the CursorAlgorithm concept

namespace cursor {
template <class T, class C>
concept CursorAlgorithm = std::move_constructible<T> /* && TODO*/;
}

int main() {
    using namespace cursor::pipes;
    using namespace cursor_library;

    static_assert(cursor::CursorAlgorithm<decltype(take(5)), numbers_from<int>>); // read: `take(5)` is a `CursorAlgrithm` that can take a `Cursor` `numbers_from<int>`
    static_assert(!cursor::CursorAlgorithm<decltype(dump), numbers_from<int>>); // make sure `dump` is not a CursorAlgorithm
}
