cmake_minimum_required(VERSION 3.0)
project(common)
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "lib")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -fPIC")
set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS}  -Wall -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}   -g3 -O0 -DCOMMON_DEBUG")   
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG  -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -g3 -O0 -DCOMMON_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG  -O2")

option (WITH_TEST_CASE "Enable Gtest" OFF)
option (WITH_CODECOVERAGE "Enable CodeCoverage" OFF)

if (WITH_CODECOVERAGE)
    message("WITH_CODECOVERAGE = ON")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

include_directories("include"
                    "src"
                    "jsoncpp/include")

set(JSONCPP_WITH_TESTS OFF)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)
set(JSONCPP_WITH_EXAMPLE OFF)
set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
set(BUILD_SHARED_LIBS OFF)
set(JSONCPP_WITH_EXAMPLE OFF)
add_subdirectory(jsoncpp)



add_subdirectory(src)


if (WITH_TEST_CASE)
    include (CTest)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(test)
endif(WITH_TEST_CASE)




