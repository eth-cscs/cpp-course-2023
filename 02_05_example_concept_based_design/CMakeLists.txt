if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_05_example_concept_based_design example_concept_based_design.md)
endif()

add_executable(02_05_cursor
    code/cursor.cpp
)
target_compile_options(02_05_cursor PRIVATE "-fconcepts-diagnostics-depth=10")
