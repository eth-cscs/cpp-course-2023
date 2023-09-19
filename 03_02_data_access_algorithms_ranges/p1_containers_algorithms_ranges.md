---
marp: true
theme: cscs
# class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---

# **Containers, algorithms, ranges**
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->

#### Peter Kardos

--- 

# C++ containers

- You must've heard about `std::vector`, `std::map`, or `std::list`
- The C++ standard library provides generic implementations of the most common data structures
- Philosophy:
    - The C++ standard specifies the properties of each container
        - For example insertion has O(log(n)) time complexity
    - Standard libraries can choose any data structure to implement the container, as long as it satisfies the specification
        - For example, `std::map` may be a red-black tree or an AVL tree
        - Or `std::unordered_map` can use any hashing policy
- Practical use:
    - Don't implement your own data structures, C++ covers 99% of your cases
    - Be aware of the remaining 1% of cases
        - C++ containers are designed to be good enough most often
        - Your application may benefit from a specialized container (e.g. Google's dense hash)

---

# The list of all containers

<div class="twocolumns">
<div>

**Sequences:**
- `std::array`
- `std::vector`
- `std::deque`
- `std::forward_list`
- `std::list`

**Associative:**
- `std::set`
- `std::map`
- `std::multiset`
- `std::multimap`

</div>
<div>

**Unordered associative:**
- `std::unordered_set`
- `std::unordered_map`
- `std::unordered_multiset`
- `std::unordered_multimap`

**Container adaptors:**
- `std::stack`
- `std::queue`
- `std::priority_queue`

**Views:**
- `std::span`
- `std::mdspan`

</div>
</div>

---

# Lesson #1: always use containers (1)

**Problem:** you need an array of increasing numbers.

**Solution (a bad one):**

```c++
int* const numbers = new int[500];
for (int i = 0; i < 100; ++i) {
    numbers[i] = i;
}
```

Oops, you **forgot the delete**!
You also **forgot to change the size of the loop** at all affected places.

---

# Lesson #1: always use containers (2)

**Better solution:**

```c++
const int count = 500;
const std::unique_ptr<int[]> numbers(new int[count]);
for (int i = 0; i < count; ++i) {
    numbers[i] = i;
}
```

Okay, it's not leaking anymore, and we also fixed the size issue.

But can we do better?

---

# Lesson #1: always use containers (3)

**Good solution:**

```c++
std::vector<int> numbers(500);
for (int i = 0; i < numbers.size(); ++i) {
    numbers[i] = i;
}
```

Why use containers even for simple things:
- They manage memory for you, and make sure it doesn't leak
- They have all the information about your data in one place
- They provide easy access to and manipulation of your data
    - Think: how are you gonna resize your array allocated with `new`?
    - Just use `std::vector::resize` instead
- The risk of you making an error is much smaller

---

# Iterators (1)

So say we have this:
```c++
std::vector<int> numbers(500);
```

I'm sure you all know this:
```c++
for (int i = 0; i < numbers.size(); ++i) {
    numbers[i] *= 2;
}
```

But do you also know this?
```c++
for (auto it = numbers.begin(); it != numbers.end(); ++it) {
    *it *= 2;
}
```

---

# Iterator concept (2)

- Iterators are generalized pointers
    - They point to one element of a *range*
    - You can dereference them with * or []
    - You can increment or decrement them, or do arithmetic
- Containers make their elements availabel as a *range*
    - An iterator to the first element is obtained by `c.begin()`
    - An iterator to the last element is obtained by `c.end()`
- They are a uniform interface accross containers
    - This helps with generic programming (e.g. iterating over a list, deque or vector is the exact same syntax)


---

# Iterators vs. pointers (3)

**Performance**:
- Zero-overhead: when compiled, iterators often optimize out to the same machine code as the equivalent pointer arithmetic

**Safety**:
- In debug builds, iterators are often checked
    - Mixing iterators of different containers is checked at runtime
    - Out of range iterators are checked at runtime

---

# C++ algorithms

- You must've heard about `std::sort`
- The C++ standard library provides generic implementations of many commonplace algorithms too
- The algorithms work on *ranges* of iterators:
    - For example, the `begin()` and `end()` iterators of you container form a range together
    - The underlying container doesn't matter, you can run the algorithms on any range
- Practical use:
    - Algorithms, together with containers, are the bread and butter of C++
    - You can implement solutions to many practical problems using them
    - They help you avoid reinventing the wheel

---

# The list of all algorithms

- There are two many to fit on a slide, but they are in two headers
- `<algorithm>`: general algorithms, like sorting, enumerating, etc.
    - Full list: https://en.cppreference.com/w/cpp/algorithm
- `<numeric>`: numerical algorithms, like reduce, inner product, etc.
    - Full list: https://en.cppreference.com/w/cpp/numeric

---

# Lesson #2: use algorithms when it makes sense (1)

**Problem:** insert an element into a sorted vector so that it stays sorted.

```c++
void InsertSorted(std::vector<int>& range, int item) {
    ...
}
```

---

# Lesson #2: use algorithms when it makes sense (2)

**Solution:** let's use binary search!

```c++
void InsertSorted(std::vector<int>& range, int item) {
    size_t a = 0;
    size_t b = range.size();
    size_t c = (a + b) / 2;
    do {
        if (item < range[c]) {
            b = c;
        }
        else {
            a = c;
        }
        c = (a + b) / 2;
    } while (a != c);
    range.insert(range.begin() + c, item);
}
```

- I haven't actually tested this code, and I'm quite confident it does NOT work
- May be performant, but looks complicated
- Binary search... come on, someone must have implemented that before me?

---

# Lesson #2: use algorithms when it makes sense (3)

Everybody used `std::sort` before, right?

```c++
void InsertSorted(std::vector<int>& range, int item) {
    range.push_back(item);
    std::sort(range.begin(), range.end());
}
```

- I'm pretty confident this solution actually works
- But it is very ineffcient, now it's O(n*log(n)) comparisons

---

# Lesson #2: use algorithms when it makes sense (4)

`std::upper_bound` is lesser known, but is just exactly what we need:

```c++
void InsertSorted(std::vector<int>& range, int item) {
    const auto location = std::upper_bound(range.begin(), range.end(), item);
    range.insert(location, item);
}
```

- I'm fairly confident this solution actually works
    - Though `std::lower_bound` and `std::upper_bound` are not trivial
- It is also simple and efficient

---

# Lesson #2: use algorithms when it makes sense (5)

Consider the following:

```c++
for (auto& item : range) {
    item *= 2;
}
```

We could also write this as:

```c++
std::for_each(range.begin(), range.end(), [](auto& item){
    item *= 2;
})
```

- The for loop is much cleaner, but in some cases, the `for_each` could also have some advantages
- Don't abuse everything into STL algorithms, do what's simple and readable

---

# Ranges: what is a range?



---

# References

- https://en.cppreference.com/w/cpp/container
- https://en.cppreference.com/w/cpp/algorithm
- https://en.cppreference.com/w/cpp/numeric

---

# Resources

<div style="text-align:center;">

Get the slides and full source code on GitHub:

<img src="../images/repo_link_qr.gif" alt="dod_nbody_example" title="" width="30%"/>

[https://github.com/eth-cscs/cpp-course-2023](https://github.com/eth-cscs/cpp-course-2023)

</div>

---