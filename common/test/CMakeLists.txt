aux_source_directory(. TEST_SRC)

add_executable(unittest ${TEST_SRC})
target_compile_definitions(unittest PRIVATE "-DUTEST -DCOMMON_DEBUG")

message("${CMAKE_BINARY_DIR}/lib")
if (WITH_CODECOVERAGE AND LINUX_)
    message("enable -lgcov")
    target_link_libraries(unittest jsoncpp pinpoint_common  gtest_main rt gcov)
elseif(APPLE)
    target_link_libraries(unittest jsoncpp pinpoint_common gtest_main)
elseif(MSVC)
    message( FATAL_ERROR "pinpoint-c-agent not support windows, if you need create an issue https://github.com/pinpoint-apm/pinpoint-c-agent/issues" )
else()
    message("------------------------------")
    target_link_libraries(unittest jsoncpp pinpoint_common gtest_main rt)
endif()

add_test(unittest unittest)