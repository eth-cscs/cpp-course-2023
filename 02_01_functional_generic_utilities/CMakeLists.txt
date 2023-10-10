if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_01_functional_generic_utilities functional_generic_utilities.md)
endif()

if (${CPP_COURSE_BUILD_CODE})
add_executable(02_01_apply
    code/apply.cpp
)

add_executable(02_01_apply_tuple_bug
    code/apply_tuple_bug.cpp
)

# clang supports parenthesized initialization of aggregates only from version 16 onwards:
# - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0960r3.html
# - https://en.cppreference.com/w/cpp/compiler_support
if(NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS_EQUAL 15.0))
    add_executable(02_00_ast
        code/ast.cpp
    )
endif()

add_executable(02_00_empty_variant_optional
    code/empty_variant_optional.cpp
)

add_executable(02_01_lambda_capture
    code/lambda_capture.cpp
)

add_executable(02_00_tuple_storage
    code/tuple_storage.cpp
)

add_executable(02_01_tuple_storage_apply
    code/tuple_storage_apply.cpp
)
endif()
