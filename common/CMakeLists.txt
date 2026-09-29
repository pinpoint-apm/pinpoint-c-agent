cmake_minimum_required(VERSION 3.13)

# Read the single source of truth for all product versions (repo root
# versions.json). The "common" version drives both the CMake project version
# and the PINPOINT_C_AGENT_API_VERSION macro injected into common.h.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../versions.json" _pp_versions_json)
string(REGEX MATCH "\"common\": *\"([0-9.]+)\"" _ "${_pp_versions_json}")
set(PINPOINT_C_AGENT_API_VERSION "${CMAKE_MATCH_1}")
if(NOT PINPOINT_C_AGENT_API_VERSION)
  message(FATAL_ERROR "Could not resolve 'common' version from versions.json")
endif()

project(pinpoint VERSION ${PINPOINT_C_AGENT_API_VERSION} DESCRIPTION  "pinpoint common library")

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(AGENT_WITH_PKGCONFIG_SUPPORT "Generate and install .pc files" OFF)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions( -DAGENT_VERSION="${PROJECT_VERSION}")

option (WITH_TEST_CASE "Enable Gtest" OFF)
option (WITH_CODECOVERAGE "Enable CodeCoverage" OFF)

if(UNIX AND NOT APPLE)
    set(LINUX_ TRUE)
endif()

if (WITH_CODECOVERAGE AND LINUX_)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic  -fPIC -Wall -Wextra")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic  -fPIC -Wall -Wextra")
endif(CMAKE_COMPILER_IS_GNUCXX)
add_subdirectory(jsoncpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)                
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/common.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/common.h @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/common.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/pinpoint/common.h @ONLY)
if (WITH_TEST_CASE)
    include (CTest)
    enable_testing()
    find_package(GTest REQUIRED)
    add_definitions(-DUTEST)
    add_subdirectory(src)
    add_subdirectory(test)
else()
    add_subdirectory(src)
endif(WITH_TEST_CASE)

if(AGENT_WITH_PKGCONFIG_SUPPORT)
    include(JoinPaths)

    join_paths(libdir_for_pc_file "\${exec_prefix}" "${CMAKE_INSTALL_LIBDIR}")
    join_paths(includedir_for_pc_file "\${prefix}" "${CMAKE_INSTALL_INCLUDEDIR}")

    configure_file("pkg-config/pinpoint_common.pc.in" "pkg-config/pinpoint_common.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pkg-config/pinpoint_common.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()
