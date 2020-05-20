aux_source_directory(. COMMON_SRC)
add_library(pinpoint_common STATIC  ${COMMON_SRC})

set_target_properties(pinpoint_common
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)