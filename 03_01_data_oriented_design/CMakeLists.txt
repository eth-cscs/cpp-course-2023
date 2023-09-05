if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(03_01_data_oriented_design data_oriented_design.md)
endif()


add_executable(03_01_false_sharing 
    code/false_sharing.cpp
)
