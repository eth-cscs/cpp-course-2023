if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(03_02_p1_containers_algorithms_ranges p1_containers_algorithms_ranges.md)
    add_marp_slides(03_02_p2_mdspan p2_mdspan.md)
endif()

if (${CPP_COURSE_BUILD_CODE})
add_executable(03_02_ranges_snippets code/ranges_snippets.cpp)
endif()
