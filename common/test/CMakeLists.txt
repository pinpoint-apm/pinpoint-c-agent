aux_source_directory(. TEST_SRC)

add_executable(unittest ${TEST_SRC})
target_compile_definitions(unittest PRIVATE "-DUTEST")
target_link_libraries(unittest jsoncpp pinpoint_common  gtest_main rt pcre)

add_test(unittest unittest)