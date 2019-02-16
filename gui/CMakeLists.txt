project(gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_definitions("/EHsc") # Flag required for Qt on Windows
endif()

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Threads REQUIRED)

set(APP_SRCS
    src/MainWindow.ui
    src/MainWindow.cpp
    src/Main.cpp
    src/DarkStyle.cpp
    src/framelesswindow/framelesswindow.ui
    src/framelesswindow/framelesswindow.cpp
    src/framelesswindow/windowdragger.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${APP_SRCS})

target_link_libraries(${PROJECT_NAME}
    ipchanger
    Qt5::Widgets
    Threads::Threads
)

if(WIN32) # Platform specific links
    target_link_libraries(${PROJECT_NAME} ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE src src/framelesswindow)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ipchanger)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug/${PROJECT_NAME}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release/${PROJECT_NAME}
)