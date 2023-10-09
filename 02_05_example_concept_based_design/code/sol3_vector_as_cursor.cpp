#include "cursor.hpp"

#include <cassert>
#include <vector>

namespace std {
template <class T, class Alloc>
T cursor_get(std::vector<T, Alloc> const& v) {
    return v[0];
}
template <class T, class Alloc>
bool cursor_done(std::vector<T, Alloc> const& v) {
    return v.size(0) > 0;
}
template <class T, class Alloc>
void cursor_next(std::vector<T, Alloc>& v) {
    std::erase(v.begin()); // question: is this a good idea?
}
} // namespace std


int main() {
    using namespace cursor::pipes;
    using namespace cursor_library;

    auto v = std::vector{ 1, 2, 3, 4, 5 };

    static_assert(Cursor<std::vector<int>>);
    v | dump;
    // prints 1, 2, 3, 4, 5
}
