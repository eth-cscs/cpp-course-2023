---
marp: true
theme: cscs
# class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---

# **Generic programming tools**
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->

#### Alberto Invernizzi, Fabian Bösch

---

# Customization mechanisms for generic interfaces

When writing generic algorithms and libraries, some information needs to be transferred from the user to library (author).
What we would like to have: customization of interfaces which

- are non-intrusive
- are explicitly opt-in
- prevent incorrect opt-in (error when wrong signature)
- are able to provide default implementations
- are simple to invoke (hard to incorrectly invoke the default when customized)
- are clear in their intent when looking at the code (what and how to customize if at all)
- are easy to verify for a given type

<br></br>
<h4></h4>
<sub><sub><sub><ol>
    <li id="footnote-1">https://brevzin.github.io/c++/2020/12/01/tag-invoke/</li>
</ol></sub></sub></sub>

---

# Dynamic Polymorphism

- Traditional way of defining interface in OOP
- works by using `virtual` functions in c++

Example:

<div class="twocolumns">
<div>

```c++
struct base {
    virtual void foo(int) = 0;
};

struct derived : base {
    virtual void foo(int) override;
};
```
</div>
<div>


can have default implementation or not (*pure virtual* function)

`override` keyword makes sure that we actually customize the interface 
(use `-Wsuggest-override`)
</div>
</div>


---

## How are we doing?

|   | virtual function | |
|---|----------------|---|
| non-intrusive | no | inheritance from base |
| explicitly opt-in | yes | have to inherit |
| prevents incorrect opt-in | yes | with `override` |
| provides default implementations | yes | non-pure base class |
| simple to invoke | yes | just call from pointer-to-base |
| hard to incorrectly invoke | yes | just call from pointer-to-base |
| code shows intent | yes | `virtual` functions show customization |
| easy to verify for a given type | yes | when `derived*` is convertible to `base*` |

Issues
- intrusive!
- otherwise fine?

---

## Other issues

- virtual function overhead
- usually requires heap allocation
- what if return type of interface is dependent on type?

<div class="twocolumns">
<div>

```c++
struct array_like {
    virtual unsigned long size() const = 0;
    virtual bool operator==(array_like const&) const = 0;
    virtual auto operator[](unsigned long) -> ????;
};
```
- We can parametrize the interface

```c++
template<typename T>
struct array_like {
    virtual unsigned long size() const = 0;
    virtual bool operator==(array_like const&) const = 0;
    virtual T& operator[](unsigned long) = 0;
};
```
</div>
<div>

<br></br>

what about this?

<br></br>

defeats purpose: have to make an interface template
for each type that we may encounter

</div>
</div>

---

# Static Polymorphism: Class Template Specialization 


- idea: specialize a class with your type

<div class="twocolumns">
<div>

```c++
// A formatter for objects of type T.
template <typename T,
          typename Char = char,
          typename Enable = void>
struct formatter {
  // A deleted default constructor indicates
  // a disabled formatter.
  formatter() = delete;
};
```
</div>
<div>

formatter class definition for `fmt::format`

from documentation: needs `parse` and `format` function
</div>
</div>

- no additional runtime overhead
- usually doesn't require allocation

---

|   | class template specialization | |
|---|----------------|---|
| non-intrusive | yes | can specialize any class |
| explicitly opt-in | yes | works only through specialization |
| prevents incorrect opt-in | partially | may still compile, but may not work correctly (concept could help) |
| provides default implementations | no | not possible (need to specialize whole class) |
| simple to invoke | yes | just call from `formatter<T>::format` |
| hard to incorrectly invoke | no | `formatter<U>::format` may just work |
| code shows intent | no | class template and `Enable` is a hint that it needs to be specialized (concept could help) |
| easy to verify for a given type | no | can check whether specialization exists (concept could help) |

---

# Static Polymorphism: Customization Points

- Familiar from standard library functions such as `std::swap`
- Work through ADL: Argument-dependent (name) lookup
- need specific incantation:

<div class="twocolumns">
<div>

```c++
using namespace std;
swap(a, b);
```
</div>
<div>

make standard namespace available
unqualified call to swap (note: not `std::swap(a, b)`)

</div>
</div>

- no additional runtime overhead
- usually doesn't require allocation

---

|   | customization points | |
|---|----------------|---|
| non-intrusive | yes | can overload `swap` for any type |
| explicitly opt-in | no | opt-in is implicit |
| prevents incorrect opt-in | partially | may still compile, but may not work correctly (concept could help) |
| provides default implementations | yes | `std::swap(...)` works for many types out of the box |
| simple to invoke | kinda | need to remember to make namespace available |
| hard to incorrectly invoke | no | use `std::swap(...)` by mistake |
| code shows intent | no | `std::swap` is just a function template in the standard library |
| easy to verify for a given type | no | only with separate concept |

---

# Static Polymorphism: Customization Point Objects

- idea: separate 
  - the piece that the user needs to specialize and
  - the piece that the user needs to invoke (*must* not specialize)
- consider this helper function:

```c++
namespace std2 {
    template<class A, class B>
        requires /* swappable constraints */
    void swap2(A& a, B& b) {
        using std::swap;
        swap(a, b);
    }
}
```

- what would we gain?

<div class="twocolumns">
<div>

```c++
using namespace std2;
swap2(a, b);
```
</div>
<div>

ADL may break this code! 

</div>
</div>

---

- Switch off ADL by using objects

```c++
namespace std2 {
    namespace hidden {
        struct swap_helper {
            template<class A, class B>
                requires /* swappable constraints */
            void operator()(A& a, B& b) const {
                using std::swap;
                swap(a, b);
            }
        };
    }
    inline constexpr hidden::swap_helper swap;
}
```

- now both ways work

<div class="twocolumns">
<div>

```c++
using namespace std2;
swap(a, b);
```
</div>
<div>

```c++
std2::swap(a, b);
```

</div>
</div>

---

|   | customization points objects | |
|---|----------------|---|
| non-intrusive | yes | can overload `swap` for any type |
| explicitly opt-in | no | opt-in is implicit |
| prevents incorrect opt-in | partially | library author can write checks (failure is earlier) |
| provides default implementations | yes | `swap_helper` *is* a default implementation |
| simple to invoke | yes | both qualified and unqualified calls work |
| hard to incorrectly invoke | yes | qualified call works equally well |
| code shows intent | no | hard to see from objects |
| easy to verify for a given type | no | only with separate concept |

Other issues
- ususally requires more code
- customization point objects reserve their identifier globally (two libraries with same CPO name may clash)

---

# Static Polymorphism: tag_invoke


---

# References

- https://brevzin.github.io/c++/2020/12/19/cpo-niebloid/
- https://brevzin.github.io/c++/2020/12/01/tag-invoke/
- https://stackoverflow.com/a/62929027
- https://quuxplusone.github.io/blog/2019/08/02/the-tough-guide-to-cpp-acronyms/#cpo
- https://www.fi.muni.cz/pv264/files/pv264_s06b_niebloids.pdf



