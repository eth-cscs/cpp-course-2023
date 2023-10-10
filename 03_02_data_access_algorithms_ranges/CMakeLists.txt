if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(03_02_containers_algorithms_ranges containers_algorithms_ranges.md)
endif()


add_executable(03_02_ranges_snippets code/ranges_snippets.cpp)