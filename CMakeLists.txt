cmake_minimum_required(VERSION 3.2)
project(common)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(common)
include_directories(common/include)

link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(test_pinpoint_cpp src/CPP/test_pinpoint.cpp)
add_executable(test_pinpoint_c src/CPP/test_pinpoint.c)
target_link_libraries(test_pinpoint_cpp pinpoint_common jsoncpp rt)
target_link_libraries(test_pinpoint_c pinpoint_common jsoncpp rt)