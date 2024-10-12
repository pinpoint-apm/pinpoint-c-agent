cmake_minimum_required(VERSION 3.15)
project(pinpoint_http_test_server)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# FetchContent_Declare(
#   pinpoint 
#   # URL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.7/pinpoint-common.tar.gz
#   URL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.7.0/pinpoint-common.tar.gz
# )

FetchContent_Declare(pinpoint SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../)

FetchContent_GetProperties(pinpoint)
if (NOT pinpoint_POPULATED)
    FetchContent_Populate(pinpoint)
    add_subdirectory(${pinpoint_SOURCE_DIR}/common ${pinpoint_BINARY_DIR})
endif ()

FetchContent_Declare(
  httplib
  URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.18.0.tar.gz
)

FetchContent_MakeAvailable(httplib pinpoint)


add_executable(server server.cpp)

target_link_libraries(server PRIVATE pthread  pinpoint_common_static httplib rt)