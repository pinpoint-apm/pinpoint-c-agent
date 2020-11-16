aux_source_directory(lib_json/ JSON_SRC)
add_library(jsoncpp STATIC ${JSON_SRC})
set_target_properties(jsoncpp PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY  ${PROJECT_BINARY_DIR}/lib)