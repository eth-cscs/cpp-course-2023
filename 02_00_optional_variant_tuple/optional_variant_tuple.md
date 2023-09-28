---
marp: true
theme: cscs
class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---

# Advanced C++ course
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->

### Essential utilities for generic programming

#### CSCS

--- 

# `std::tuple` is simple, why do we care? 

- Generic programming in C++ is in many cases "remove as many constraints as
  possible": the fewer constraints, the more generic
  - _Don't overdo it, if there isn't a use case for it_
- But, sometimes we introduce constraints without realizing it
- This and the next session will cover a basic set of C++ tools that are useful
  in generic programming
  - Using them correctly is important for both memory safety and performance

--- 

# Overview

- `std::tuple` for storing a compile-time known number of potentially homogeneous types
  - by far the most commonly used utility in generic programming
- `std::variant` for storing one of a compile-time known number of potentially homogeneous types
- `std::optional` for storing up to one type

---

# `std::tuple`: what is it not good for?

- If all elements in the tuple are known to be of the same type, prefer `std::array`

```c++
std::array<int, 3> a{42, 43, 44};

// not
std::tuple<int, int, int> t{42, 43, 44};
```

---

# `std::tuple`: what is it not good for?

- If you can give names to the members, prefer a struct

```c++
struct interval {
    double begin;
    double end;
};

interval i{1.0, 13.5};
// use i.begin and i.end

// not
using interval = std::tuple<double, double>
interval i{1.0, 13.5};
// use std::get<0>(i) and std::get<1>(i)
```

---

# `std::tuple`: what is it good for? Generic programming!

- `std::tuple<Ts...>` :thumbsup:
- `std::tuple<T1, T2>`:shrug:
- `std::tuple<int, double>` :thumbsdown:

---

# `std::tuple`: what is it good for?

- `std::tuple<Ts...>`: great for storing packs for later use
- TODO: Add an example use case

```c++
template <typename... Ts>
struct mytype {
    // Unfortunately we can't do this
    // Ts... ts;
    // But we can do this
    std::tuple<Ts...> ts;
};
```

---

# `std::tuple`: constructing

| `Ts...` | `auto t =` | `decltype(t)` |
| -- | -- | -- |
| `int, double&, mytype` | `std::tuple(ts...)` | `std::tuple<int, double, mytype>` |
| `int, double&, mytype` | `std::make_tuple(ts...)` | `std::tuple<int, double, mytype>` |
| `int, double&, mytype` | `std::tuple<Ts...>(ts...)` | `std::tuple<int, &double, mytype>` |
| `int, double&, mytype` | `std::tuple<std::remove_cvref_t<Ts>...>(ts...)` | `std::tuple<int, double, mytype>` |
| `int, double&, mytype` | `std::forward_as_tuple(ts...)` | `std::tuple<int&, double&, mytype&>` |

---

# `std::tuple`: constructing but with forwarding

| `Ts...` | `auto t =` | `decltype(t)` |
| -- | -- | -- |
| `int, double&, mytype` | `std::tuple(std::forward<Ts>(ts)...)` | `std::tuple<int, double, mytype>` |
| `int, double&, mytype` | `std::make_tuple(std::forward<Ts>(ts)...)` | `std::tuple<int, double, mytype>` |
| `int, double&, mytype` | `std::tuple<Ts...>(std::forward<Ts>(ts)...)` | `std::tuple<int, &double, mytype>` |
| `int, double&, mytype` | `std::tuple<std::remove_cvref_t<Ts>...>(std::forward<Ts>(ts)...)` | `std::tuple<int, double, mytype>` |
| `int, double&, mytype` | `std::forward_as_tuple(std::forward<Ts>(ts)...)` | `std::tuple<int&&, double&, mytype&&>` |
  
--- 

# Capturing a pack in a lambda

<div class="twocolumns">
<div>

## Since C++20:
```c++
template <typename... Ts>
void f(Ts&&... ts) {
    auto ff1 = [ts...]() {};
    auto ff2 = [...ts = std::forward<Ts>(ts)]() {};
}
```

</div>
<div>

## In C++17:
```c++
template <typename... Ts>
void f(Ts&&... ts) {
    auto ff1 = [t = std::tuple(ts...)]() {};
    auto ff2 = [t = std::tuple(std::forward<Ts>(ts)...)]() {};
}
```

<div>
<div>

---

# `std::optional`

- Traditional use case: failure

```c++
template <typename T>
T div(T x, T y) noexcept {
    return x / y; // May be undefined behaviour
}

template <typename T>
std::optional<T> safeish_div(T x, T y) noexcept {
    if (y == T{0}) { return std::nullopt; }
    else { return x / y; }
}
```

- However, `std::expected` (C++23) or exceptions are generally still a better choice because they cal tell you _why_ something failed

---

# `std::optional`

- Traditional use case: missing value

```c++
std::vector<int> v1;
int x1 = v1.pop_back(); // Undefined behaviour

myvector<int> v2;
// Use of x2 requires explicit checking that it's valid
std::optional<int> x2 = v2.pop_back();
```

---

# `std::optional`

- Generic programming use case: storing non-default-constructible types

<div class="twocolumns">
<div>

```c++
struct mytype {
    mytype() = delete;
    mytype(int x) : x(x) {}
    // copy and move constructors/assignment
};
template <typename T>
struct mycontainer {
    // Requires that T is default-constructible
    T x;
};
// mycontainer<mytype> c{}; // not ok
mycontainer<mytype> c{mytype(42)}; // ok
```

</div>
<div>

```c++
struct mytype {
    mytype() = delete;
    mytype(int x) : x(x) {}
    // copy and move constructors/assignment
};
template <typename T>
struct mycontainer2 {
    // x can be filled in later without constraints on T
    std::optional<T> x;
};
mycontainer<mytype> c{}; // ok
mycontainer<mytype> c{mytype(42)}; // ok
```

</div>
</div>

---

# `std::variant`

- Closed set of homogeneous types, one is active
- Like union, but type safe

<div class="twocolumns">
<div>

Setting

```c++
std::variant<int, std::string> v{"hello"};
// The second alternative is now active
std::print("Alternative {} is active\n", v.index());
// Is the first alternative active?
std::print("Is the first alternative active: {}\n",
    std::holds_alternative<T>(v));
```

</div>
<div>

Access
```c++
// Access with std::get
std::string x1 = std::get<1>(v); // ok
std::string x2 = std::get<std::string>(v); // ok
// throws std::bad_variant_access
// std::string y1 = std::get<0>(v);
// throws std::bad_variant_access
// std::string y2 = std::get<int>(v);
```

Visiting
```c++
// Or with a generic function
void myvisitor(int) { /* got an int */ }
void myvisitor(std::string) { /* got a string */ }
std::visit(myvisitor, v);
```

</div>
</div>

---

# `std::variant`

- Use case: Implementing `std::optional`!
* ```c++
  template <typename T>
  struct optional {
      std::variant<std::monostate, T> v;
  };
  ```

---

# `std::variant`

- Use case: abstract syntax tree
- Full example: https://godbolt.org/z/ecTE1x6h9

```c++
template <typename... Ts>
using sp = std::shared_ptr<Ts...>;

struct add; struct mul; struct lit { int x; };
using ast = std::variant<lit, sp<add>, sp<mul>>;
struct add { ast x, y; };
struct mul { ast x, y; };

int eval(ast a) {
    std::visit(overloaded(
        [](lit& l) { return l.x; },
        [](sp<add>& a) { return eval(a->x) + eval(a->y); },
        [](sp<mul>& m) { return eval(m->x) * eval(m->y); }),
        a)
}
```

---

# `std::variant`

- `std::monostate`: an empty tag type that can be used to make `std::variant` default constructible

```c++
// First type is active after default construction
// Fails to compile if mytype is not default constructible
std::variant<mytype, int> v;
```

```c++
// Always compiles, no matter what mytype is
std::variant<std::monostate, mytype, int> v;
```

---

# Unpacking a tuple

TODO: Move to function objects etc. session?

```c++
std::tuple<int, std::string> t;
std::apply([](auto&&... ts) {
    // We now have access to a pack ts
    f(tag_t{}, std::forward<decltype(ts)>(ts)..., 42);
}, std::move(t));
```

---

# Bonus exercise: implement `apply`

- TODO: May belong in other generic programming session
- Example implementation: https://godbolt.org/z/7e4b7sKz5

---

# Enjoy your break: How does the following work?

TODO: Move to function objects etc. session?

```c++
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

int main() {
    std::variant<int, std::string> v{"hi!"};
    std::visit(overloaded([](int x) { std::cout << "variant holds " << x << '\n'; },
                          [](std::string x) { std::cout << "variant holds " << x << '\n'; }),
               v)
}
```
