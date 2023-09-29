if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_05_examples_sid examples_sid.md)
endif()

add_executable(02_05_sid
    code/sid.cpp
)
target_compile_options(02_05_sid PRIVATE "-fconcepts-diagnostics-depth=10")
