project(IPChanger-GUI)
set(CMAKE_CXX_STANDARD 17)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${IPCHANGER_INCLUDE_DIR})

# Populate a CMake variable  with the sources
set(APP_SRCS
    MainWindow.ui
    MainWindow.cpp
    Main.cpp
    MainWindow.h
)

add_executable(${PROJECT_NAME} ${APP_SRCS})
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Threads::Threads
    boost_system
    boost_filesystem
)
