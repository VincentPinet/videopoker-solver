file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.cpp")
foreach(source ${sources})
    cmake_path(GET source FILENAME name)
    cmake_path(REMOVE_EXTENSION name)
    string(PREPEND name "${CMAKE_PROJECT_NAME}-test-")
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE "${CMAKE_PROJECT_NAME}-lib" GTest::gtest GTest::gtest_main --coverage)
  	target_compile_options(${name} PRIVATE --coverage)
    gtest_discover_tests(${name} XML_OUTPUT_DIR "gtest-result" DISCOVERY_TIMEOUT 42)
endforeach()
