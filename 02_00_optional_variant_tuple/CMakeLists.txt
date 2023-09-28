if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_00_optional_variant_tuple optional_variant_tuple.md)
endif()

# clang supports parenthesized initialization of aggregates only from version 16 onwards:
# - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0960r3.html
# - https://en.cppreference.com/w/cpp/compiler_support
if(NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS_EQUAL 15.0))
    add_executable(02_00_ast
        code/ast.cpp
    )
endif()
