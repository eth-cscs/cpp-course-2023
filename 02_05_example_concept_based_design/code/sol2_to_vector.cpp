#include "cursor.hpp"

#include <cassert>
#include <vector>

constexpr inline auto to_vector = [](cursor::Cursor auto&& cur_) {
    std::vector<std::decay_t<decltype(cursor::get(cur_))>> res;
    for (auto v : cur_) {
        res.push_back(v);
    }
    return res;
};

int main() {
    using namespace cursor::pipes;
    using namespace cursor_library;

    auto res = numbers_from(2) | take(5) | to_vector;

    static_assert(std::is_same_v<std::vector<int>, std::decay_t<decltype(res)>>);
    assert((res == std::vector<int>{ 2, 3, 4, 5, 6 }));
}
