file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.cpp")
add_library("${CMAKE_PROJECT_NAME}-lib" ${sources})
target_include_directories("${CMAKE_PROJECT_NAME}-lib" PUBLIC include)
