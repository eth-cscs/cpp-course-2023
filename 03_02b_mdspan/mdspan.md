---
marp: true
theme: cscs
# class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---

# Advanced C++ Course
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->

### std::mdspan

#### CSCS

--- 

# Motivation

- C-Layout vs Fortran-Layout
- vocabulary type

HPC software dealing with multi-dimensional data:
- allocation of md array
- accessing md array
- iteration over md array

How to interface between libraries?
- Best case: well-defined/described concept that can be modelled by the *other* library
- Worst case: concrete class

---

# What is std::mdspan?

`std::mdspan ` is a non-owning multi-dimensional array view

- since C++23, see https://wg21.link/p0009
![](md_span_implementation_status.png)
- think of *pointer* and *metadata* (how to interpret the pointed-to memory)

```c++
template<
    class T,
    class Extents,
    class LayoutPolicy = std::layout_right,
    class AccessorPolicy = std::default_accessor<T>
> class mdspan;
```

- *TriviallyCopyable*\*: can be used in host/device interfaces
- allows different layouts

\* under some costraints

---

# Layout example*

```c++
using layout = /* see-below */;

std::mdspan<double, std::extents<int, N, M>, layout> A = ...;
std::mdspan<double, std::extents<int, N>> y = ...;
std::mdspan<double, std::extents<int, M>> x = ...;

std::ranges::iota_view range{0, N};

std::for_each(std::execution::par_unseq, 
  std::ranges::begin(range), std::ranges::end(range),
  [=](int i) {
     double sum = 0.0;
     for(int j = 0; j < M; ++j) {
       sum += A[i, j] * x[j];
     }
     y[i] = sum;
  });
```

- on CPUs: C-layout aka row-major aka std::layout_right performs well (vectorized inner loop)
- on GPUs: Fortran-layout aka column-major aka std::layout_left performs well (coalesced memory load)

\* from https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0009r18.html 2.6

---

# More about Layout

custom layout allow
- to skip elements (useful for tiling)
- map multiple indices to the same element

---

# Custom accessor example

prevent to use device mdspan on host

---

# submdspan

- not part of C++23, but in working draft for C++26
