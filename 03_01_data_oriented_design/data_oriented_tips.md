---
marp: true
theme: cscs
# class: lead
paginate: true
backgroundColor: #fff
backgroundImage: url('../slides-support/common/4k-slide-bg-white.png')
size: 16:9
---
# General Hints
* Minimize memory usage
    * pack, use smaller types
    * in some cases even compression can help
    * but beware of alignement
* Bools and enums in structures
    * try to avoid if/case in the element operation
    * separate elements by types
* Centralize handling
    * move memory management into a centralized system (system, environment,...)
    * Operate on bunch of elements, not on the single element
        * amortize load cost on several similar operations, ideally working on close by memory
        * global optimizations are easier to do
* Think about a SoA (Struct of Arrays) instead of AoS (Arrays of struct)
* prefer Handles/indexes to pointers when referring to elements

---
# References

- Handles are the better pointers
https://floooh.github.io/2018/06/17/handles-vs-pointers.html
- Andrew Kelly [Practical DOD](https://vimeo.com/649009599)
- CppCon 2014: Mike Acton "Data-Oriented Design and C++": [youtube.com/watch?v=rX0ItVEVjHc](https://youtube.com/watch?v=rX0ItVEVjHc)
- Richard Fabian, 'Data-Oriented Design': [dataorienteddesign.com/dodbook/](https://www.dataorienteddesign.com/dodbook/)
- IT Hare, 'Infographics: Operation Costs in CPU Clock Cycles': [ithare.com/infographics-operation-costs-in-cpu-clock-cycles/](http://ithare.com/infographics-operation-costs-in-cpu-clock-cycles/)
- The Brain Dump, 'Handles are the better pointers': [floooh.github.io/2018/06/17/handles-vs-pointers.html](https://floooh.github.io/2018/06/17/handles-vs-pointers.html)