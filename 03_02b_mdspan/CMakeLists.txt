if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(03_02b_mdspan mdspan.md)
endif()

include(FetchContent)
FetchContent_Declare(
    mdspan
    GIT_REPOSITORY https://github.com/kokkos/mdspan.git
    GIT_TAG        stable
)
FetchContent_MakeAvailable(mdspan)

add_executable(03_02b_mdspan code/mdspan.cpp)
target_link_libraries(03_02b_mdspan mdspan)
target_compile_features(03_02b_mdspan PRIVATE cxx_std_23)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${CMAKE_BINARY_DIR}
        FILES_MATCHING PATTERN "*.png")
