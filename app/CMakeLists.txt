add_executable("${CMAKE_PROJECT_NAME}-app" src/main.cpp)
target_link_libraries("${CMAKE_PROJECT_NAME}-app" "${CMAKE_PROJECT_NAME}-lib")