if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(01_02_infrastructure_resources infrastructure_resources.md)
endif()

add_subdirectory(code/cpp-example)
