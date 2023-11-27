if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(0x_0x_coroutines coroutines.md)
endif()

add_executable(0x_0x_task code/task.cpp)