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

### Future C++

#### CSCS

--- 

# C++20, 23, 26, and beyond

- New features added regularly
- Easier than ever to get newer compilers
- Language gets more complex, but at the same time simpler if you restrict yourself
- Editorial note: whatever features we cover earlier in the course should of course be removed from here

---

# Overview?

- `std::execution`
- `std::simd`
- `std::linalg` + `std::mdspan`
- `std::expected`
- `std::print`

---

# `std::execution`

- Until now: parallel algorithms (CPU only)
- Third party vendor solutions:
  - Thrust (CPU, NVIDIA, AMD)
  - nvhpc (NVIDIA)
  - SYCL (Intel, NVIDIA, AMD)
- Other third party libraries:
  - Kokkos
  - Alpaka
- No generic solution, and no clear winner emerging
- Generic building blocks in C++ standard

---

# `std::execution`

```
placeholder for an interesting std::execution example
```

---

# `std::execution`

- Performance portable building blocks
- Asynchronous code as a first class citizen
- *Senders* represent work
- *Schedulers* represent where work runs
- *Algorithms* represent what work to do
- Low-level building blocks on target to be included in C++26
- Reference implementation stdexec supports CPU and CUDA
- pika developed at CSCS to build on top of the reference implementation
- Asynchronous parallel algorithms still need to be added on top of `std::execution`

---

# `std::simd`

- ...

---

# `std::linalg`

- Decades of existing practice
- No more ZGERC
- Instead: matrix_rank_1_update_c(std::par, x, y, A)
- Works with mdspan as inputs
- Execution policies for parallelization
- GPU support dependent on `std::execution` for parallel algorithms

---

# `std::linalg`

```
placeholder for interesting std::linalg example
```

---

# `std::expected`

- ...

---

# `std::print`

- ...
