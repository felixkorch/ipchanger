cmake_minimum_required(VERSION 3.8.1)
set(CMAKE_CXX_STANDARD 17)

project(IPChanger)

find_package(Boost COMPONENTS system REQUIRED)

set(IPCHANGER_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/lib")
set(IPCHANGER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(IPCHANGER_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${IPCHANGER_INCLUDE_DIR})

add_subdirectory(gui)
