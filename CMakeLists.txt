cmake_minimum_required(VERSION 3.8.1)
set(CMAKE_CXX_STANDARD 17)

include_directories(ipchanger/src)
include_directories(".")

add_subdirectory(ipchanger)
add_subdirectory(sandbox)
add_subdirectory(gui)