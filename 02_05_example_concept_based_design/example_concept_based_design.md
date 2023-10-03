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

#### Hannes Vogt (idea Anton Afanasyev)

--- 

# Motivation

## Why?
TODO

## Example


```c++
struct some_cursor {
    int const& get() const;
    bool done() const;
    void next();
};
```

see a full example https://godbolt.org/z/5bx7xazYa


---

# Summary
