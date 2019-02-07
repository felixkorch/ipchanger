project(ipchangergui)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
if(MSVC)
    ADD_DEFINITIONS("/EHsc")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif(MSVC)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Threads REQUIRED)

set(APP_SRCS
    src/MainWindow.ui
    src/MainWindow.cpp
    src/Main.cpp
    src/MainWindow.h
    src/DarkStyle.cpp
    src/framelesswindow/framelesswindow.cpp
    src/framelesswindow/windowdragger.cpp
)

if(WIN32)
    add_executable(ipchangergui WIN32 ${APP_SRCS})
    target_link_libraries(ipchangergui
        ipchangerlib
        Qt5::Widgets
        Threads::Threads
        ws2_32
    )
else()
    add_executable(ipchangergui ${APP_SRCS})
    target_link_libraries(ipchangergui
        ipchangerlib
        Qt5::Widgets
        Threads::Threads
        stdc++fs
    )
endif()

target_include_directories(ipchangergui PUBLIC src)
target_include_directories(ipchangergui PUBLIC src/framelesswindow)

set_target_properties(ipchangergui PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
 
install (TARGETS ipchangergui
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)