project(ipchanger)

set(APP_SRCS
    src/ippch.cpp
    src/ipchanger/Changer.cpp
    src/ipchanger/Resolver.cpp
    src/ipchanger/System.cpp
)

if(WIN32) # Add platform specific files
    set(APP_SRCS
        ${APP_SRCS}
        src/ipchanger/platform/windows/System_Windows.cpp
        src/ipchanger/platform/windows/Process_Windows.cpp
    )
else()
    set(APP_SRCS
        ${APP_SRCS}
        src/ipchanger/platform/linux/System_Unix.cpp
        src/ipchanger/platform/linux/Process_Unix.cpp
    )
endif()

add_library(${PROJECT_NAME} ${APP_SRCS})
#add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(WIN32) # Platform specific links
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug/${PROJECT_NAME}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release/${PROJECT_NAME}
)