aux_source_directory(. TEST_SRC)

add_executable(unittest ${TEST_SRC})
target_compile_definitions(unittest PRIVATE "-DUTEST -DCOMMON_DEBUG")


if (WITH_CODECOVERAGE)
    message("enable -lgcov")   
    target_link_libraries(unittest jsoncpp_static pinpoint_common  gtest_main rt pcre gcov)
else()
    target_link_libraries(unittest jsoncpp_static pinpoint_common  gtest_main rt pcre)
endif()

add_test(unittest unittest)