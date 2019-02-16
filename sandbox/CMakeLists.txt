project(sandbox)

set(APP_SRCS
    src/Sandbox.cpp
)

add_executable(${PROJECT_NAME} ${APP_SRCS})

target_link_libraries(${PROJECT_NAME} ipchanger)

#if(WIN32) # Platform specific links
#    target_link_libraries(${PROJECT_NAME} ws2_32)
#else()
#    target_link_libraries(${PROJECT_NAME} stdc++fs)
#endif()

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug/${PROJECT_NAME}
#    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release/${PROJECT_NAME}
#)