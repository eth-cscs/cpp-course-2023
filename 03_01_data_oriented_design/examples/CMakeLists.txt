cmake_minimum_required(VERSION 3.10)

project(StructLayout VERSION 1.0)
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)


add_executable(03_01_struct_layout1
    struct_layout.cpp
)
target_link_libraries(03_01_struct_layout1 PRIVATE Catch2::Catch2WithMain)
