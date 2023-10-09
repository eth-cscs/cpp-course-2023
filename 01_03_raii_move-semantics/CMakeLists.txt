if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(01_03_raii_move-semantics raii_move-semantic.md)
endif()

add_executable(01_03_raii_buffer
    code/raii_buffer.cpp
)
