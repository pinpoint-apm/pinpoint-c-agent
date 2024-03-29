file(GLOB_RECURSE TEST_SRC
  "${PROJECT_SOURCE_DIR}/src/test_*.cc"
)
add_executable(TestCommon ${TEST_SRC} test_main.cc)
target_include_directories(TestCommon PUBLIC ${PROJECT_SOURCE_DIR}/jsoncpp/include)
if (WITH_CODECOVERAGE AND LINUX_)
# message("enable -lgcov")
target_link_libraries(TestCommon pinpoint_common_static gtest_main rt gcov )
else()
target_link_libraries(TestCommon pinpoint_common_static gtest_main)
endif()

add_test(TestCommon TestCommon)
add_executable(main test_main.cpp)
target_link_libraries(main pinpoint_common_static)