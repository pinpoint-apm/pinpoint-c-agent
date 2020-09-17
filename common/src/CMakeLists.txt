LIST(APPEND src_dir . NodePool Cache Context Util ConnectionPool)
foreach(dir  ${src_dir})
    aux_source_directory(${dir} SRC_FILES)
endforeach()

add_library(pinpoint_common STATIC ${SRC_FILES})

set_target_properties(pinpoint_common
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
)