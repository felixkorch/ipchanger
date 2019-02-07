project(sandbox)

set(APP_SRCS
    src/Sandbox.cpp
)

if(WIN32)
    add_executable(${PROJECT_NAME} ${APP_SRCS})
    target_link_libraries(${PROJECT_NAME}
        ipchangerlib
        ws2_32
    )
else()
    add_executable(${PROJECT_NAME} ${APP_SRCS})
    target_link_libraries(${PROJECT_NAME}
        ipchangerlib
        stdc++fs
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
 
install (TARGETS ${PROJECT_NAME}
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)