cmake_minimum_required(VERSION 3.8.1)
project(IPChanger-Console)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost COMPONENTS system program_options REQUIRED)

add_executable(${PROJECT_NAME} Main.cpp)

target_link_libraries(${PROJECT_NAME} boost_system boost_program_options pthread)
