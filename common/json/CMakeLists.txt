aux_source_directory(src/ JSON_SRC)

add_library(jsoncpp STATIC ${JSON_SRC})
set_target_properties(jsoncpp
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)