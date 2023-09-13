cmake_minimum_required(VERSION 3.25)

project(cpp_course_2023)

option(CPP_COURSE_BUILD_SLIDES "Weather to build slides." ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if (${CPP_COURSE_BUILD_SLIDES})
    include(cmake/Marp.cmake)
endif()


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	set(CPP_COURSE_AVX_OPTION "-march=core-avx2")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CPP_COURSE_AVX_OPTION "-march=core-avx2")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")
    set(CPP_COURSE_AVX_OPTION "-march=core-avx2")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	set(CPP_COURSE_AVX_OPTION "/arch:AVX2")
endif()


add_subdirectory(01_01_introduction)
add_subdirectory(01_02_infrastructure_resources)
add_subdirectory(01_03_initialization_etc)
add_subdirectory(01_04_optional_variant_tuple)

add_subdirectory(02_01_lambdas_etc)
add_subdirectory(02_02_generic_programming_part1)
add_subdirectory(02_03_generic_programming_part2)
add_subdirectory(02_04_exercises)

add_subdirectory(03_01_data_oriented_design)
add_subdirectory(03_02_data_access_algorithms_ranges)
add_subdirectory(03_03_examples_something_something)
add_subdirectory(03_04_future)