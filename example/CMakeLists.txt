cmake_minimum_required(VERSION 3.30)
project(simple_jpeg_example)

set(CMAKE_CXX_STANDARD 23)

add_executable(simple_jpeg_example EXCLUDE_FROM_ALL main.cpp)
target_link_libraries(simple_jpeg_example PRIVATE simple_jpeg)