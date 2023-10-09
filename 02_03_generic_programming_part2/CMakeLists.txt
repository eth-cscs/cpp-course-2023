if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_03_generic_programming_part2 generic_programming_part2.md)
endif()

add_executable(02_03_swap
    code/swap.cpp
)

add_executable(02_03_swap_cpo
    code/swap_cpo.cpp
)

add_executable(02_03_swap_tag_invoke
    code/swap_tag_invoke.cpp
)
