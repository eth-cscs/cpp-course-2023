if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_05_example_concept_based_design example_concept_based_design.md)
endif()

if (${CPP_COURSE_BUILD_CODE})
add_executable(02_05_test_cursor code/test_cursor.cpp)
add_test(NAME 02_05_test_cursor COMMAND 02_05_test_cursor)

add_executable(02_05_sol1_filter code/sol1_filter.cpp)
add_test(NAME 02_05_sol1_filter COMMAND 02_05_sol1_filter)

add_executable(02_05_sol2_to_vector code/sol2_to_vector.cpp)
add_test(NAME 02_05_sol2_to_vector COMMAND 02_05_sol2_to_vector)
endif()
