aux_source_directory(lib_json/ JSON_SRC)
add_library(jsoncpp_static STATIC  ${JSON_SRC})

target_include_directories(jsoncpp_static
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:include>
)