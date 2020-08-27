aux_source_directory(. COMMON_SRC)
aux_source_directory(NodePool COMMON_NODE_SRC)
add_library(pinpoint_common STATIC  ${COMMON_SRC} ${COMMON_NODE_SRC})

set_target_properties(pinpoint_common
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)