if (${CPP_COURSE_BUILD_SLIDES})
    add_marp_slides(02_01_lambdas_etc lambdas_etc.md)
endif()

add_executable(02_01_apply
    code/apply.cpp
)

add_executable(02_01_tuple_storage_apply
    code/tuple_storage_apply.cpp
)

add_executable(02_01_lambda_capture
    code/lambda_capture.cpp
)
