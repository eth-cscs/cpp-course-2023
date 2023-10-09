#include "cursor.hpp"

#include <cassert>
#include <type_traits>
#include <vector>

namespace cursor {
template <class T, class C>
concept CursorAlgorithm = std::move_constructible<T> && requires(T alg, C cur) {
    {
        alg(cur)
        } -> Cursor;
};
} // namespace cursor

int main() {
    using namespace cursor::pipes;
    using namespace cursor_library;

    static_assert(cursor::CursorAlgorithm<decltype(take(5)), numbers_from<int>>);
    static_assert(!cursor::CursorAlgorithm<decltype(dump), numbers_from<int>>);
}
