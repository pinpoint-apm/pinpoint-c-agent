file(GLOB_RECURSE TEST_SRC
  "${PROJECT_SOURCE_DIR}/src/test_*.cc"
)
add_executable(TestCommon ${TEST_SRC} test_main.cc)
target_compile_definitions(TestCommon PRIVATE "-DUTEST -DCOMMON_DEBUG")

# message("${CMAKE_BINARY_DIR}/lib")
if (WITH_CODECOVERAGE AND LINUX_)
    # message("enable -lgcov")
    target_link_libraries(TestCommon pinpoint_common_static gtest_main rt gcov )
elseif(APPLE)
    target_link_libraries(TestCommon pinpoint_common_static gtest_main)
elseif(MSVC)
    message( FATAL_ERROR "pinpoint-c-agent not support windows, if you need create an issue https://github.com/pinpoint-apm/pinpoint-c-agent/issues" )
else()
    message("------------------------------")
    target_link_libraries(TestCommon pinpoint_common_static gtest_main rt)
endif()

add_test(TestCommon TestCommon)