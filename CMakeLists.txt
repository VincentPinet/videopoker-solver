cmake_minimum_required(VERSION 3.20)

project(videopoker-solver VERSION 0.1.0 LANGUAGES CXX)

option(ENABLE_TESTING "Build the unit tests" ON)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wall -Wextra -Wpedantic -Werror -march=native -O3)

add_subdirectory(lib)
add_subdirectory(app)

if (ENABLE_TESTING)
  find_package(GTest REQUIRED)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()