---
marp: true
theme: cscs
# class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---

# **Marp**
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->

### Generic Programming part I

#### everyone + JB

--- 

# What do we mean by Generic Programming
Wikipedia: "Generic programming is a style of computer programming in which algorithms are written in terms of data types to-be-specified-later that are then instantiated when needed for specific types provided as parameters."

Us:  
* Code re-use
    - (Design) Patterns
* Abstraction of algorithms and data
    - Look no further than the STL (it _is_ amazing)
* API  
    - Standardize the API for algorithms/operations
* Use of operators and predicates

---
# Evolution of code
* Once upon a time
    - Reusable pieces of code (or Cut'n'Paste)
* Function call mechanism
    - Enables recursion and callbacks
* Runtime polymorphisms
    - Runtime only protections
* Multiple versions of compiled libraries
    - RTTI
    - Does not work well for strongly typed languages
* C++ Generic Libraries: static polymorphism
    - Main mechanism for overhead-free C++ abstractions

# 

<div class="twocolumns">
<div>

## Code

```c++
void f(int x) {
}
```

</div>
<div>

## Explanations

declaration of a function f...

</div>
</div>

---

# Ideas

Variant with a visitor - using `auto`
 ```c++
    // get the i-th param from the variant algorithm list
    auto p = std::visit([=](auto const& obj) { 
        return obj.params[i];
    }, alg);
    // draw datasets in left column, params in right
    int column = std::visit([&](auto const& v) { 
        return get_column(v); 
    }, std::get<1>(p));  ```

