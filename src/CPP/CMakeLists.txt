cmake_minimum_required(VERSION 3.13)
project(common)
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
# FetchContent_Declare(
#   pinpoint
#   GIT_REPOSITORY https://github.com/pinpoint-c-agent/pinpoint-c-agent.git
#   # GIT_TAG        74bc39d813d664cb56b78b1506d91932c8131396 
#   # not recommended, please use hash key like `74bc39d813d664cb56b78b1506d91932c8131396`
#   GIT_TAG        origin/dev
# )

FetchContent_Declare(pinpoint SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../)

FetchContent_GetProperties(pinpoint)
if (NOT pinpoint_POPULATED)
    FetchContent_Populate(pinpoint)
    add_subdirectory(${pinpoint_SOURCE_DIR}/common ${pinpoint_BINARY_DIR})
endif ()

FetchContent_MakeAvailable(pinpoint)

add_executable(test_pinpoint_cpp test_pinpoint.cpp)
add_executable(test_pinpoint_c test_pinpoint.c)

if(LINUX_)
target_link_libraries(test_pinpoint_cpp PRIVATE  pinpoint_common_static rt)
target_link_libraries(test_pinpoint_c pinpoint_common_static rt)
else()
target_link_libraries(test_pinpoint_cpp PRIVATE  pinpoint_common_static )
target_link_libraries(test_pinpoint_c  PRIVATE pinpoint_common_static )
endif()