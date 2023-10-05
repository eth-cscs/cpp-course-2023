---
marp: true
theme: cscs
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
--- 
<style>   
div.center-padded {
    width: 50%;
    margin: 0 auto;
}
</style>

# **Concepts**
![bg cover](../slides-support/common/title-bg3.png)
<!-- _paginate: skip  -->
<!-- _class: titlecover -->
<!-- _footer: "" -->

### Generic Programming
Alberto Invernizzi, CSCS (alberto.invernizzi@cscs.ch)

---
<!-- _class: lead -->
# Why do we need generic programming?

---
# Strong Typing

C++ is a strongly typed language.

This means that each variable is assigned a type at definition, and it cannot change over time.

This gives a lot of safety, plus it allows the language(=compiler) to do assumptions and optimize the code.

---
# 💪 Strong typing vs 🦆 duck typing?

<div class="center-padded">

<!-- https://godbolt.org/z/41KdEd333 -->

```python
class Person:
    def feed(self, food):
        print(f"lets eat some {food}")

class Chimney:
    def feed(self, fuel):
        print(f"let's burn some {fuel}")

def feed_all(storage, obj):
    for element in storage:
        obj.feed(element)

storage_basement = ["🪵", "⛽️"]
storage_1st_floor = ["🍝", "🍗", "🧁",]

print("not a big problem... 🗑️")
feed_all(storage_1st_floor, Chimney())

print("unless you start eating it! 💀")
feed_all(storage_basement, Person())
```

```
not a big problem... 🗑️
let's burn some 🍝
let's burn some 🍗
let's burn some 🧁
unless you start eating it! 💀
lets eat some 🪵
lets eat some ⛽️
```

<!--
```cpp
template <class T, class U>
void enroll(T x, U y) {
    x.feed(y);
}

enroll(Student(), CppAdvancedCourse());
enroll(Customer(), PremiumProgram());
```
-->

</div>

---
<!-- _class: lead -->

# It's better safe than sorry

---
<!-- _class: lead -->

# It's better **type**safe than sorry
(cit)

---

<div class="center-padded">

Yeah, but having to write a function/class for every single type (and combination) does not scale...

```cpp
int subtract(unsigned int a, unsigned int b);
int subtract(int a, int b);
int subtract(int a, unsigned int b);
float subtract(float a, float b);
```

<center>

**Oh! That's why we need generic programming!**

</center>

</div>

---
<center>

## C++ GENERIC PROGRAMMING = TEMPLATE!

</center>

<div class="center-padded">

```cpp
template <class T>
T add(T a, T B) {
    return a + b;
}

auto add(auto a, auto B) {
    return a + b;
}
```

</div>

We can have template specialization for different types and combinations, but the template "placeholder" accepts anything. If at template instantiation time, it addresses a problem with the given type, it will complain with a build error 💥.

Basic template generic programming sounds a bit like "duck typing", but at compile time.

It's slightly better, but in C++ we are not satisfied with sub-optimal solutions...we want the best! 😁

--- 
# SFINAE

<div class="center-padded">

**SFINAE** allows to disable/enable some overloads at certain conditions.


```cpp
template <class T,
    std::enable_if_t<
            std::is_integral_v<T>
            or std::is_integral_v<T>
            , int> = 0>
void add(T a, T b) {
    return a + b;
}
```

With **SFINAE** we have a finer control on the overload set, in practice **it enables constraining template**! 

<center>

**Exactly what we wanted!**

</center>

</div>

---
# SFINAE is powerful and is supported by STL

Actually, SFINAE allows us to do many things in a quite rigorous way.

SFINAE is supported by the STL with:

- `std::enable_if`
  - it can be used in many ways to enable or disable a specialization (class, function, ...)
- `#include <type_traits>`
  - it provides some common and useful requirements and transformers for types

---
# SFINAE errors

<div class="center-padded">

It's nice that we can figure out at compile time of errors instead of runtime...we like it! It means less error in production, safer code. Nice!

```cpp
#include <vector>
#include <algorithm>

struct Number {
    long long value_;
};

int main() {
    std::vector<int> integers;
    std::sort(integers.begin(), integers.end());

    std::vector<Number> numbers;
    std::sort(numbers.begin(), numbers.end());
}
```

There's an error in the code above...

</div>

---
<style scoped>
pre { 
    color: white;
    white-space: pre-wrap;
    font-size: 5pt;
}
</style>

# GCC 13.2 - 79 lines of output

<!-- https://godbolt.org/z/34nq5K3rK -->

<pre>
In file included from /opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algobase.h:71,
                 from /opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/vector:62,
                 from <source>:1:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h: In instantiation of 'constexpr bool __gnu_cxx::__ops::_Iter_less_iter::operator()(_Iterator1, _Iterator2) const [with _Iterator1 = __gnu_cxx::__normal_iterator<Number*, std::vector<Number> >; _Iterator2 = __gnu_cxx::__normal_iterator<Number*, std::vector<Number> >]':
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1819:14:   required from 'void std::__insertion_sort(_RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1859:25:   required from 'void std::__final_insertion_sort(_RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1950:31:   required from 'void std::__sort(_RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:4861:18:   required from 'void std::sort(_RAIter, _RAIter) [with _RAIter = __gnu_cxx::__normal_iterator<Number*, vector<Number> >]'
<source>:13:14:   required from here
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:45:23: error: no match for 'operator<' (operand types are 'Number' and 'Number')
   45 |       { return *__it1 < *__it2; }
      |                ~~~~~~~^~~~~~~~
In file included from /opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algobase.h:67:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1250:5: note: candidate: 'template<class _IteratorL, class _IteratorR, class _Container> bool __gnu_cxx::operator<(const __normal_iterator<_IteratorL, _Container>&, const __normal_iterator<_IteratorR, _Container>&)'
 1250 |     operator<(const __normal_iterator<_IteratorL, _Container>& __lhs,
      |     ^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1250:5: note:   template argument deduction/substitution failed:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:45:23: note:   'Number' is not derived from 'const __gnu_cxx::__normal_iterator<_IteratorL, _Container>'
   45 |       { return *__it1 < *__it2; }
      |                ~~~~~~~^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1258:5: note: candidate: 'template<class _Iterator, class _Container> bool __gnu_cxx::operator<(const __normal_iterator<_Iterator, _Container>&, const __normal_iterator<_Iterator, _Container>&)'
 1258 |     operator<(const __normal_iterator<_Iterator, _Container>& __lhs,
      |     ^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1258:5: note:   template argument deduction/substitution failed:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:45:23: note:   'Number' is not derived from 'const __gnu_cxx::__normal_iterator<_Iterator, _Container>'
   45 |       { return *__it1 < *__it2; }
      |                ~~~~~~~^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h: In instantiation of 'bool __gnu_cxx::__ops::_Val_less_iter::operator()(_Value&, _Iterator) const [with _Value = Number; _Iterator = __gnu_cxx::__normal_iterator<Number*, std::vector<Number> >]':
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1799:20:   required from 'void std::__unguarded_linear_insert(_RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Val_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1827:36:   required from 'void std::__insertion_sort(_RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1859:25:   required from 'void std::__final_insertion_sort(_RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1950:31:   required from 'void std::__sort(_RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:4861:18:   required from 'void std::sort(_RAIter, _RAIter) [with _RAIter = __gnu_cxx::__normal_iterator<Number*, vector<Number> >]'
<source>:13:14:   required from here
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:98:22: error: no match for 'operator<' (operand types are 'Number' and 'Number')
   98 |       { return __val < *__it; }
      |                ~~~~~~^~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1250:5: note: candidate: 'template<class _IteratorL, class _IteratorR, class _Container> bool __gnu_cxx::operator<(const __normal_iterator<_IteratorL, _Container>&, const __normal_iterator<_IteratorR, _Container>&)'
 1250 |     operator<(const __normal_iterator<_IteratorL, _Container>& __lhs,
      |     ^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1250:5: note:   template argument deduction/substitution failed:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:98:22: note:   'Number' is not derived from 'const __gnu_cxx::__normal_iterator<_IteratorL, _Container>'
   98 |       { return __val < *__it; }
      |                ~~~~~~^~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1258:5: note: candidate: 'template<class _Iterator, class _Container> bool __gnu_cxx::operator<(const __normal_iterator<_Iterator, _Container>&, const __normal_iterator<_Iterator, _Container>&)'
 1258 |     operator<(const __normal_iterator<_Iterator, _Container>& __lhs,
      |     ^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1258:5: note:   template argument deduction/substitution failed:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:98:22: note:   'Number' is not derived from 'const __gnu_cxx::__normal_iterator<_Iterator, _Container>'
   98 |       { return __val < *__it; }
      |                ~~~~~~^~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h: In instantiation of 'bool __gnu_cxx::__ops::_Iter_less_val::operator()(_Iterator, _Value&) const [with _Iterator = __gnu_cxx::__normal_iterator<Number*, std::vector<Number> >; _Value = Number]':
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_heap.h:140:48:   required from 'void std::__push_heap(_RandomAccessIterator, _Distance, _Distance, _Tp, _Compare&) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Distance = long int; _Tp = Number; _Compare = __gnu_cxx::__ops::_Iter_less_val]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_heap.h:247:23:   required from 'void std::__adjust_heap(_RandomAccessIterator, _Distance, _Distance, _Tp, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Distance = long int; _Tp = Number; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_heap.h:356:22:   required from 'void std::__make_heap(_RandomAccessIterator, _RandomAccessIterator, _Compare&) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1635:23:   required from 'void std::__heap_select(_RandomAccessIterator, _RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1910:25:   required from 'void std::__partial_sort(_RandomAccessIterator, _RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1926:27:   required from 'void std::__introsort_loop(_RandomAccessIterator, _RandomAccessIterator, _Size, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Size = long int; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:1947:25:   required from 'void std::__sort(_RandomAccessIterator, _RandomAccessIterator, _Compare) [with _RandomAccessIterator = __gnu_cxx::__normal_iterator<Number*, vector<Number> >; _Compare = __gnu_cxx::__ops::_Iter_less_iter]'
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_algo.h:4861:18:   required from 'void std::sort(_RAIter, _RAIter) [with _RAIter = __gnu_cxx::__normal_iterator<Number*, vector<Number> >]'
<source>:13:14:   required from here
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:69:22: error: no match for 'operator<' (operand types are 'Number' and 'Number')
   69 |       { return *__it < __val; }
      |                ~~~~~~^~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1250:5: note: candidate: 'template<class _IteratorL, class _IteratorR, class _Container> bool __gnu_cxx::operator<(const __normal_iterator<_IteratorL, _Container>&, const __normal_iterator<_IteratorR, _Container>&)'
 1250 |     operator<(const __normal_iterator<_IteratorL, _Container>& __lhs,
      |     ^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1250:5: note:   template argument deduction/substitution failed:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:69:22: note:   'Number' is not derived from 'const __gnu_cxx::__normal_iterator<_IteratorL, _Container>'
   69 |       { return *__it < __val; }
      |                ~~~~~~^~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1258:5: note: candidate: 'template<class _Iterator, class _Container> bool __gnu_cxx::operator<(const __normal_iterator<_Iterator, _Container>&, const __normal_iterator<_Iterator, _Container>&)'
 1258 |     operator<(const __normal_iterator<_Iterator, _Container>& __lhs,
      |     ^~~~~~~~
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/stl_iterator.h:1258:5: note:   template argument deduction/substitution failed:
/opt/compiler-explorer/gcc-13.2.0/include/c++/13.2.0/bits/predefined_ops.h:69:22: note:   'Number' is not derived from 'const __gnu_cxx::__normal_iterator<_Iterator, _Container>'
   69 |       { return *__it < __val; }
      |                ~~~~~~^~~~~~~
Compiler returned: 1
</pre>

---
<style scoped>
pre { 
    color: white;
    white-space: pre-wrap;
    font-size: 12pt;
}
</style>

# Clang 17.0.1 - (extent of) 9 errors, 200+ lines of output

<!-- https://godbolt.org/z/jYPszxnhz -->

<pre>
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/predefined_ops.h:69:22: error: invalid operands to binary expression ('Number' and 'Number')
   69 |       { return *__it < __val; }
      |                ~~~~~ ^ ~~~~~
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_heap.h:140:42: note: in instantiation of function template specialization '__gnu_cxx::__ops::_Iter_less_val::operator()<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, Number>' requested here
  140 |       while (__holeIndex > __topIndex && __comp(__first + __parent, __value))
      |                                          ^
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_heap.h:247:12: note: in instantiation of function template specialization 'std::__push_heap<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, long, Number, __gnu_cxx::__ops::_Iter_less_val>' requested here
  247 |       std::__push_heap(__first, __holeIndex, __topIndex,
      |            ^
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_heap.h:356:9: note: in instantiation of function template specialization 'std::__adjust_heap<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, long, Number, __gnu_cxx::__ops::_Iter_less_iter>' requested here
  356 |           std::__adjust_heap(__first, __parent, __len, _GLIBCXX_MOVE(__value),
      |                ^
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_algo.h:1635:12: note: in instantiation of function template specialization 'std::__make_heap<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, __gnu_cxx::__ops::_Iter_less_iter>' requested here
 1635 |       std::__make_heap(__first, __middle, __comp);
      |            ^
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_algo.h:1910:12: note: in instantiation of function template specialization 'std::__heap_select<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, __gnu_cxx::__ops::_Iter_less_iter>' requested here
 1910 |       std::__heap_select(__first, __middle, __last, __comp);
      |            ^
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_algo.h:1926:13: note: in instantiation of function template specialization 'std::__partial_sort<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, __gnu_cxx::__ops::_Iter_less_iter>' requested here
 1926 |               std::__partial_sort(__first, __last, __last, __comp);
      |                    ^
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_algo.h:1947:9: note: in instantiation of function template specialization 'std::__introsort_loop<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, long, __gnu_cxx::__ops::_Iter_less_iter>' requested here
 1947 |           std::__introsort_loop(__first, __last,
      |                ^
/opt/compiler-explorer/gcc-13.2.0/lib/gcc/x86_64-linux-gnu/13.2.0/../../../../include/c++/13.2.0/bits/stl_algo.h:4861:12: note: in instantiation of function template specialization 'std::__sort<__gnu_cxx::__normal_iterator<Number *, std::vector<Number>>, __gnu_cxx::__ops::_Iter_less_iter>' requested here
 4861 |       std::__sort(__first, __last, __gnu_cxx::__ops::__iter_less_iter());
      |            ^
</pre>

---
# 🌹 Every rose has its torn

I haven't said that SFINAE was fantastic...

Don't get me wrong: it is a super tool, but it looks more like a workaround than a proper tool of the language. 

The typical *"it's not a bug is a feature"* applied to the C++ language. Morover, SFINAE has some limitations (e.g. there is no place for it in constructors).

Different techinques and language evolutions overcome some of this limitations improving this situation:
*tag dispatching*, `constexpr`, and ...

<center>

# CONCEPTS
### C++20™️

</center>

---
# Concepts

TODO quote from Bjarne Stroustrup

+ nothing dramatically new
+ just a more readable way for SFINAE
+ that does not look like an incident

They introduce some new language keywords and construct:
- `requires`
- `concept`

---
<!-- _class: lead -->

# Ready?

---
# Template

<div class="center-padded">

<!-- https://godbolt.org/z/rT11vxh43 -->

```cpp
#include <type_traits>
#include <vector>

template <class Float>
Float mean(const Float a, const Float b) {
    return (a + b) / 2;
}

float res_00 = mean(2.0, 3.0);      // 2.5
double res_01 = mean(2.0, 3.0);     // 2.5

float res_02 = mean(1, 2);          // 1
int res_03 = mean('a', 'd');        // 98 (= 'b')

// compile error: std::vector does not have `+`
std::vector<float> res_v = mean(
    std::vector<float>{1,2,3},
    std::vector<float>{4,5,6});
```

👍 no of code duplication thanks to templates!

👎 one fits all...unconstrained!
👎 error message is not straightforward

</div>

---
# SFINAE
<div class="center-padded">

```cpp
#include <type_traits>

template <class Float,
          class = std::enable_if_t<std::is_floating_point_v<Float>>>
Float mean(const Float a, const Float b) {
    return (a + b) / 2;
}

float res_00 = mean(2.0, 3.0);
double res_01 = mean(2.0, 3.0);

// compile error
// float res_02 = mean(1, 2);
// int res_03 = mean('a', 'd');
```

```bash
...
error: no type named 'type' in 'struct std::enable_if<false, void>'
 2514 |     using enable_if_t = typename enable_if<_Cond, _Tp>::type;
      |           ^~~~~~~~~~~
```

🍾 `Float` is now constrained!

👎 code readability is affected
🤨 error message is "a bit" cryptic

</div>

---
# Concepts
<div class="center-padded">

```cpp
#include <concepts>
template <std::floating_point Float>
Float mean(const Float a, const Float b) {
    return (a + b) / 2;
}

float res_00 = mean(2.0, 3.0);
double res_01 = mean(2.0, 3.0);

// compile error
// float res_02 = mean(1, 2);
```

```bash
error: no matching function for call to 'mean(int, int)'
   15 |     float res_02 = mean(1, 2);
      |                    ~~~~^~~~~~
...
required for the satisfaction of 'floating_point<Float>' [with Float = int]
note: the expression 'is_floating_point_v<_Tp> [with _Tp = int]' evaluated to 'false'
  111 |     concept floating_point = is_floating_point_v<_Tp>;
      |                              ^~~~~~~~~~~~~~~~~~~~~~~~
```
😉 same semantic
👍 better error message
👍 better code readability

</div>

---
# SFINAE ⏩ CONCEPTS

<div class="twocolumns">
<div>

### SFINAE

```cpp
template <class Float,
          class = std::enable_if_t<std::is_floating_point_v<Float>>>
Float mean(const Float a, const Float b) {
    return (a + b) / 2;
}
```

</div>
<div>

### Concepts

```cpp
template <std::floating_point Float>
Float mean(const Float a, const Float b) {
    return (a + b) / 2;
}
```

</div>
</div>

**<mark>We didn't introduce any new language keyword (yet), and we already achieved a more terse and readable code, expressing exactly the same thing!</mark>**

A couple of notes:

<center>

||SFINAE|Concepts|
|-|:-:|:-:|
|STL definitions|`#include <type_traits>`|`#include <concepts>`|
|Names|verb (e.g. `is_floating_point`)|adjective (e.g. `floating_point`)|
</center>

---
<!-- _class: lead -->
# Exploring Concepts

---
<div class="center-padded">

```cpp
template <std::floating_point Float>
Float mean(const Float a, const Float b) {
    return (a + b) / 2;
}
```

In this way we defined a named placeholder `Float`, on which we constrain to be a `std::floating_point`.

This syntax can be used directly "in-place" using `auto` for creating the placeholder

```cpp
std::floating_point auto mean(
    const std::floating_point auto a,
    const std::floating_point auto b) {
        return (a + b) / 2;
}
```

**Are they semantically the same?**
*🧐 hint: how many placeholders there are?*

</div>

---
<div class="center-padded">

```cpp
template <std::floating_point Float>
Float mean(const Float a, const Float b) {
    return (a + b) / 2;
}

float res_00 = mean(2.0, 3.0);
double res_01 = mean(2.0, 3.0);

float res_02 = mean<float>(2.0f, 3.0);

// compiler error
float res_03 = mean(2.0f, 3.0);
```

</div>

```bash
<source>: In function 'int main()':
<source>:28:24: error: no matching function for call to 'mean(float, double)'
   28 |     float res_03 = mean(2.0f, 3.0);
      |                    ~~~~^~~~~~~~~~~
<source>:10:7: note: candidate: 'template<class Float>  requires  floating_point<Float> Float mean(Float, Float)'
   10 | Float mean(const Float a, const Float b) {
      |       ^~~~
<source>:10:7: note:   template argument deduction/substitution failed:
<source>:28:24: note:   deduced conflicting types for parameter 'Float' ('float' and 'double')
   28 |     float res_03 = mean(2.0f, 3.0);
      |                    ~~~~^~~~~~~~~~~
```

---
# Multiple placeholder

<div class="center-padded">

This fixes the problem of different types for arguments, because they can be deduced separately.

```cpp
template <
    std::floating_point FloatA,
    std::floating_point FloatB>
float mean(const FloatA a, const FloatB b) {
    return (a + b) / 2;
}

float res_00 = mean(2.0, 3.0);
double res_01 = mean(2.0, 3.0);
float res_02 = mean(2.0f, 3.0);
```

But now the return type is fixed to `float`...

If we add a placeholder `FloatR`, since it cannot deduce the return type, it has to be explicitly indicated in the call!

</div>

---
# ret-type contract

<div class="center-padded">

Without constraints this is correct, since the floating point type used for `a` will be implicitly cast to `int`.

```cpp
int floor(const std::floating_point auto a) {
    return a;
}
```

Here we are constraining the return type by asking it to be integral...

```cpp
std::integral auto floor(const std::floating_point auto a) {
    return a;
}
```

```bash
...
<source>:29:16: error: deduced return type does not satisfy placeholder constraints
   29 |         return a;
      |                ^
<source>:29:16: note: constraints not satisfied
...
...
<concepts>:102:24: note: the expression 'is_integral_v<_Tp> [with _Tp = float]' evaluated to 'false'
  102 |     concept integral = is_integral_v<_Tp>;
      |                        ^~~~~~~~~~~~~~~~~~
```

</div>

---
<div class="center-padded">

We didn't see much about concepts, but they already proved to be very useful! 😍

```cpp
<concept> <type>
```

Just by using them like this, we can easily constrain a type (better, a placeholder of a type, e.g. `auto`).

We've already seen them in action in various places for functions, lastly for return types, but also for arguments...

Are arguments so different from variable definition!? Nope! Actually **we can use concepts also for variable definition!**

```cpp
const std::integral auto res = mean(1.0f, 2.0f);
```

If `mean` returns a `float`, this is not going to build!

</div>

---
<!-- _class: lead -->

# Concepts
(syntax and new language constructs)

---
require-expression

---
require clause

---
concept keyword

---
concepts guideline

+ naming
+ big topic, not single function
+ partial (for debugging, but also it's ok to start partial and refine later, it's hard)

---
Where to put cv qualified with concepts

---
<!-- _class: lead -->

# Concepts in reality

---
# Type erasure
`std::function<>` vs `std::invocable`

---
# Static polymorphism

With **Templates+SFINAE** we can achieve static polymorphism.

*Static polymorphism*, contrarily to its dual *dynamic polymorphism*, happens at **compile time**.

It has some nice implications:
+ errors are raised at compile time
+ no overhead at runtime (e.g. no `virtual` function call)

---
# Recap

+ Why generic programming?
+ SFINAE -> CONCEPTS Step by Step
+ Concepts (new syntax and definition of custom concepts)
+ Applications (type erasure, static polymorphism, ...)

---
<!-- _class: lead -->
# Q&A

Alberto Invernizzi
Research Software Engineer @ CSCS