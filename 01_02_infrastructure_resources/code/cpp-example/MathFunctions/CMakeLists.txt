Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

# create the MathFunctions library
add_library(MathFunctions MathFunctions.cxx)

# state that anybody linking to us needs to include the current source dir
# to find MathFunctions.h, while we don't.
target_include_directories(MathFunctions
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                           )
 
# link our compiler flags interface library
target_link_libraries(MathFunctions PUBLIC tutorial_compiler_flags)

add_executable(testmathfunctions test_MathFunctions.cxx)
target_link_libraries(testmathfunctions PUBLIC MathFunctions tutorial_compiler_flags)
target_link_libraries(testmathfunctions PRIVATE Catch2::Catch2WithMain)
