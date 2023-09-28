if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_00_optional_variant_tuple optional_variant_tuple.md)
endif()

add_executable(02_00_ast
    code/ast.cpp
)
