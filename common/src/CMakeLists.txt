LIST(APPEND src_dir . NodePool Cache Context Util ConnectionPool)
foreach(dir  ${src_dir})
    aux_source_directory(${dir} SRC_FILES)
endforeach()

add_library(pinpoint_common_obj OBJECT ${SRC_FILES})
add_library(pinpoint_common STATIC $<TARGET_OBJECTS:pinpoint_common_obj> $<TARGET_OBJECTS:jsoncppObj>)
