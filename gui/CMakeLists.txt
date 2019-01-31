project(IPChanger)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${IPCHANGER_INCLUDE_DIR})
include_directories(framelesswindow)

set(APP_SRCS
    MainWindow.ui
    MainWindow.cpp
    Main.cpp
    MainWindow.h
    DarkStyle.cpp
    framelesswindow/framelesswindow.cpp
    framelesswindow/windowdragger.cpp
)

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${APP_SRCS})
    target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Threads::Threads
        ws2_32
        stdc++fs
    )
else()
    add_executable(${PROJECT_NAME} ${APP_SRCS})
    target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Threads::Threads
        stdc++fs
    )
endif()
