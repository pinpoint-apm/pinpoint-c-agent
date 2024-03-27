file(GLOB_RECURSE SRC_FILES
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(pinpoint_common_static STATIC ${SRC_FILES})
target_link_libraries(pinpoint_common_static jsoncpp_static)

target_include_directories(pinpoint_common_static PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
)

add_library(pinpoint_common_shared SHARED ${SRC_FILES}) 
target_link_libraries(pinpoint_common_shared jsoncpp_static)

install(DIRECTORY include/ DESTINATION include/pinpoint_common
        FILES_MATCHING PATTERN "*.h")

set_target_properties(pinpoint_common_shared pinpoint_common_static PROPERTIES 
        VERSION ${PROJECT_VERSION} 
        SOVERSION 1
        PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../include/common.h")      

install(TARGETS pinpoint_common_shared pinpoint_common_static ${INSTALL_EXPORT}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pinpoint_common
)