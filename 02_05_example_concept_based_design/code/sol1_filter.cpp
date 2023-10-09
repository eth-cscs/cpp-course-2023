#include "cursor.hpp"

template <cursor::Cursor C, class Fun>
class filter_impl {
    C cur_;
    Fun fun_;

    void advance_to_next() {
        for (; !fun_(cursor::get(cur_)) && !cursor::done(cur_); cursor::next(cur_))
            ;
    }

public:
    template <cursor::Cursor D>
    filter_impl(D&& cur, Fun fun) : cur_{ std::forward<D>(cur) }, fun_{ std::move(fun) } {
        // set cursor to first filtered in value
        advance_to_next();
    }


    friend decltype(auto) cursor_get(filter_impl const& cur) {
        return cursor::get(cur.cur_);
    }
    friend bool cursor_done(filter_impl const& cur) {
        return cursor::done(cur.cur_);
    }
    friend void cursor_next(filter_impl& cur) {
        cursor::next(cur.cur_);
        cur.advance_to_next();
    }
};

constexpr inline auto filter = [](auto fun) {
    return [fun = std::move(fun)]<cursor::Cursor C>(C&& cur) {
        return filter_impl<C, decltype(fun)>(std::forward<C>(cur), std::move(fun));
    };
};

int main() {
    using namespace cursor::pipes;
    using namespace cursor_library;

    auto odd_predicate = [](auto a) { return a % 2 == 1; };

    auto odd = filter(odd_predicate);

    numbers_from(2) | odd | take(5) | dump;
    numbers_from(2) | take(5) | odd | dump;
    numbers_from(3) | take(5) | odd | dump;

    auto n = numbers_from(3);
    n | odd | take(2) | dump;
    n | odd | take(2) | dump;
}
