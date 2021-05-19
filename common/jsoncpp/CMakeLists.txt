aux_source_directory(lib_json/ JSON_SRC)
add_library(jsoncppObj OBJECT  ${JSON_SRC})
# add_library(jsoncpp STATIC $<TARGET_OBJECTS:jsoncppObj>)