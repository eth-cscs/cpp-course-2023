if (${CPP_COURSE_BUILD_SLIDES})
    #    add_marp_slides(01_04_generic_programming_part1 generic_programming_part1.md)
    add_marp_slides(01_04_templates_part1 templates_part1.md)
endif()

add_executable(01_04_fold_print code/fold_print.cpp)

add_executable(01_04_debug_print_specializations code/debug_print_specializations.cpp)
target_include_directories(01_04_debug_print_specializations PUBLIC code)
