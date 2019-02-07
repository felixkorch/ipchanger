project(ipchangerlib)

if(MSVC)
    ADD_DEFINITIONS("/EHsc")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif(MSVC)

set(APP_SRCS
    src/pch.cpp
    src/ipchanger/Changer.cpp
    src/ipchanger/system/Resolver.cpp
    src/ipchanger/system/System.cpp
)

if(WIN32)

    set(APP_SRCS
        ${APP_SRCS}
        src/ipchanger/system/platform/windows/System.Windows.cpp
        src/ipchanger/system/platform/windows/WindowsProcess.cpp
    )

    add_library(ipchangerlib ${APP_SRCS})
    target_link_libraries(ipchangerlib
        ws2_32
    )
else()

    set(APP_SRCS
        ${APP_SRCS}
        src/ipchanger/system/platform/linux/System.Unix.cpp
        src/ipchanger/system/platform/linux/UnixProcess.cpp
    )

    add_library(ipchangerlib ${APP_SRCS})
    target_link_libraries(ipchangerlib
        stdc++fs
    )
endif()

target_include_directories(ipchangerlib PUBLIC src src/ipchanger src/ipchanger/system)

set_target_properties(ipchangerlib PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
 
install (TARGETS ipchangerlib
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install
         ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/_install
)