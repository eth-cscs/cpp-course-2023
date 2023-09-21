add_executable(03_01_nbody)
target_sources(03_01_nbody
    PRIVATE
        main.cpp
        common.hpp
        oop.cpp
        oop.hpp
        dod.cpp
        dod.hpp
)

target_compile_options(03_01_nbody PRIVATE ${CPP_COURSE_MATH_NO_ERRNO_OPTION})
target_compile_options(03_01_nbody PRIVATE ${CPP_COURSE_AVX_OPTION})
target_compile_options(03_01_nbody PRIVATE "-fno-vectorize")