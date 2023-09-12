if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(03_01_data_oriented_design data_oriented_design.md)
endif()


add_executable(03_01_dram_burst_mode
    code/dram_burst_mode.cpp
)

add_executable(03_01_dram_block_size
    code/dram_block_size.cpp
)

add_executable(03_01_cache_false_sharing 
    code/cache_false_sharing.cpp
)