file(GLOB_RECURSE SRC_FILES
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_library(pinpoint_common_obj OBJECT ${SRC_FILES})
# add_library(pinpoint_common STATIC $<TARGET_OBJECTS:pinpoint_common_obj> $<TARGET_OBJECTS:jsoncppObj>)
