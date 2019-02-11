project(ipchanger)

set(APP_SRCS
    src/pch.cpp
    src/ipchanger/Changer.cpp
    src/ipchanger/system/Resolver.cpp
    src/ipchanger/system/System.cpp
)

if(WIN32) # Add platform specific files
    set(APP_SRCS
        ${APP_SRCS}
        src/ipchanger/system/platform/windows/System.Windows.cpp
        src/ipchanger/system/platform/windows/WindowsProcess.cpp
    )
else()
    set(APP_SRCS
        ${APP_SRCS}
        src/ipchanger/system/platform/linux/System.Unix.cpp
        src/ipchanger/system/platform/linux/UnixProcess.cpp
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
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug/${PROJECT_NAME}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release/${PROJECT_NAME}
)