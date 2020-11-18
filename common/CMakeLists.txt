cmake_minimum_required(VERSION 3.0)
project(common)
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -fPIC")
set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS}  -Wall -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}   -g3 -O0 -DCOMMON_DEBUG")   
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG  -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -g3 -O0 -DCOMMON_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG  -O2")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option (WITH_TEST_CASE "Enable Gtest" OFF)
option (WITH_CODECOVERAGE "Enable CodeCoverage" OFF)

if (WITH_CODECOVERAGE)
    # message("WITH_CODECOVERAGE = ON")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

include_directories("include"
                    "src"
                    "jsoncpp/include")
                    
link_directories(${CMAKE_BINARY_DIR}/lib)

add_subdirectory(jsoncpp)
add_subdirectory(src)

if (WITH_TEST_CASE)
    include (CTest)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(test)
endif(WITH_TEST_CASE)




