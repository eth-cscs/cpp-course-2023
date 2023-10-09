cmake_minimum_required(VERSION 3.25)

project(cpp_course_2023 CXX)

#------------------------------------------------------------------------------
# Setup Output directories
#------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single Directory for all Libraries"
)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single Directory for all static libraries."
)

option(CPP_COURSE_BUILD_SLIDES "Weather to build slides." ON)
option(CPP_COURSE_BUILD_SLIDES_PDF "Weather to build slides." ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (${CPP_COURSE_BUILD_SLIDES})
    include(cmake/Marp.cmake)
endif()


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	set(CPP_COURSE_AVX_OPTION "-march=core-avx2")
    set(CPP_COURSE_MATH_NO_ERRNO_OPTION "-fno-math-errno")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CPP_COURSE_AVX_OPTION "-march=core-avx2")
    set(CPP_COURSE_MATH_NO_ERRNO_OPTION "-fno-math-errno")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
    set(CPP_COURSE_AVX_OPTION "-march=core-avx2")
    set(CPP_COURSE_MATH_NO_ERRNO_OPTION "")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	set(CPP_COURSE_AVX_OPTION "/arch:AVX2")
    set(CPP_COURSE_MATH_NO_ERRNO_OPTION "")
endif()


# Day 1
add_subdirectory(01_01_introduction)
add_subdirectory(01_02_infrastructure_resources)
add_subdirectory(01_03_initialization_etc)
add_subdirectory(01_04_generic_programming_1_templates)

# Day 2
add_subdirectory(02_01_functional_generic_utilities)
add_subdirectory(02_03_generic_programming_part2)
add_subdirectory(02_04_exercises)
add_subdirectory(02_05_example_concept_based_design)

# Day 3
add_subdirectory(03_01_data_oriented_design)
add_subdirectory(03_02_data_access_algorithms_ranges)
add_subdirectory(03_02b_mdspan)
add_subdirectory(03_03_examples_something_something)
add_subdirectory(03_04_future)
