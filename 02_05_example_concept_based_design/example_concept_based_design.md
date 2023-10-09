---
marp: true
theme: cscs
# class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---

# **Advanced C++ course 2023**
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->

### Concept based design by example

#### CSCS (designed by Anton Afanasyev)

--- 

# Motivation

<!--
Survey: level of experience with templates and generic programming
- who used templates, instantiated templates?
- who implemented a templated class or function?
- who worked on a generic library? with template heavy code? with template meta-programming?
-->

> Concepts (requirements on template arguments) are the central feature of C++ generic library design; they define the terms in which a library’s generic data structures and algorithms are specified. Every working generic library is based on concepts. These concepts may be represented using specifically designed language features, in requirements tables, as comments in the code, in design documents, or simply in the heads of programmers. However, without concepts (formal or informal), no generic code could work.

(from "Design of Concept Libraries for C++" by Andrew Sutton, Bjarne Stroustrup)

> Generic Programming doesn't mean templates. It means generalizing algorithm implementations iteratively, discovering sets of requirements on their arguments and grouping the requirements into named concepts and hierarchies of concepts. It's about algorithms, not templates.

(Eric Niebler on X, Jan 8th 2020)

**Writing generic libraries requires discipline!**

--- 


# Example Concept for today


```c++
struct some_cursor {
    int const& get() const;
    bool done() const;
    void next();
};
```

We will develop a library around that concept: [start](https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:3,endLineNumber:9,positionColumn:3,positionLineNumber:9,selectionStartColumn:3,selectionStartLineNumber:9,startColumn:3,startLineNumber:9),source:'%23include+%3Cconcepts%3E%0A%23include+%3Ciostream%3E%0A%23include+%3Ctype_traits%3E%0A%0Astruct+some_cursor+%7B%0A++++int+const%26+get()+const%3B%0A++++bool+done()+const%3B%0A++++void+next()%3B%0A%7D%3B%0A%0Aint+main()+%7B%0A++++std::cout+%3C%3C+%22Hello+Cursor!!%22+%3C%3C+std::endl%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:49.852045256745,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:executor,i:(argsPanelShown:'1',compilationPanelShown:'0',compiler:g132,compilerName:'',compilerOutShown:'0',execArgs:'',execStdin:'',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-std%3Dc%2B%2B20+-Wpedantic+-fsanitize%3Daddress,undefined',overrides:!(),source:1,stdinPanelShown:'1',wrap:'1'),l:'5',n:'0',o:'Executor+x86-64+gcc+13.2+(C%2B%2B,+Editor+%231)',t:'0')),k:50.147954743255,l:'4',n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),version:4)

---

# Exercises


starting point for exercises https://godbolt.org/z/WPcGhY976

---

# Summary: Cursor Concept

```c++
struct some_cursor {
    int const& get() const;
    bool done() const;
    void next();
};

template <class T> concept Cursor =
    std::move_constructible<T> && requires(T& cursor, T const& const_cursor) {
        cursor.next();
        { const_cursor.done() } -> std::convertible_to<bool>;
        const_cursor.get();
    };

static_assert(Cursor<some_cursor>);
```

---
# Summary: Cursor Algorithms

```c++
template <std::integral T>
struct numbers_from {
    T value_;
    T const& get() const { return value_; }
    void next() { ++value_; }
    bool done() const { return false; }
};

struct take_impl{/* ... */};
constexpr inline auto take = [](int n) {
    return [n](Cursor auto cur) { return take_impl(std::move(cur), n); };
};

dump(take(2)(numbers_from(42)))
```
---

# Summary: Improving Syntax: Composition and Pipe

```c++
namespace pipes {
    template <class F, class G>
    constexpr auto operator|(F&& f, G&& g) -> decltype(auto) {
        if constexpr (std::is_invocable_v<G&&, F&&>) {
            return g(f);
        }
        else {
            return [g = std::forward<G>(g), f = std::forward<F>(f)](auto&&... args) {
                return g(f(std::forward<decltype(args)...>(args...)));
            };
        }
    }
}; // namespace pipes
```
---

# Summary: Customization Points

```c++
namespace cursor {
// default done
auto cursor_done(...) { return false; }

// cursor fallback
auto cursor_done(auto const& cur) -> decltype(cur.done()) { return cur.done(); }
auto cursor_next(auto& cur) -> decltype(cur.next()) { cur.next(); }
auto cursor_get(auto const& cur) -> decltype(cur.get()) { return cur.get(); }

// customization point
constexpr inline auto done = [](auto const& cur) -> decltype(cursor_done(cur)) { return cursor_done(cur); };
constexpr inline auto next = [](auto& cur) -> decltype(cursor_next(cur)) { cursor_next(cur); };
constexpr inline auto get = [](auto const& cur) -> decltype(cursor_get(cur)) { return cursor_get(cur); };
}
```
---

# Summary: Type Erasure

```c++
template <class T> class any_cursor {
    struct iface {
        virtual ~iface(){};
        virtual T get() const = 0;
        virtual bool done() const = 0;
        virtual void next() = 0;
    };

    template <Cursor C> struct impl : iface {
        C cur_;
        impl(C cur) : cur_(std::move(cur)) {}
        T get() const { return cursor::get(cur_); }
        bool done() const { return cursor::done(cur_); }
        void next() { cursor::next(cur_); }
    };
    std::unique_ptr<iface> impl_;

public:
    template <class C> any_cursor(C cur) : impl_{ new impl<C>(std::move(cur)) } {}
    friend auto cursor_done(any_cursor const& cur) -> decltype(auto) { return cur.impl_->done(); }
    friend auto cursor_next(any_cursor& cur) -> decltype(auto) { cur.impl_->next(); }
    friend auto cursor_get(any_cursor const& cur) -> decltype(auto) { return cur.impl_->get(); }
};
```

---

# Summary: range-based for loop 

```c++
struct sentinel {};

template <Cursor C>
struct iter {
    C& cur_;

    decltype(auto) operator*() const {
        return cursor::get(cur_);
    }
    void operator++() {
        cursor::next(cur_);
    }
    bool operator!=(sentinel) const {
        return !cursor::done(cur_);
    }
};

template <Cursor C>
iter<C> begin(C& cur) { return { cur }; }
template <Cursor C>
sentinel end(C const& cur) { return {}; }
```
