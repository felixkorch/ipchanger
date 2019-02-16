cmake_minimum_required(VERSION 3.8.1)
set(CMAKE_CXX_STANDARD 17)

if(!MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

add_subdirectory(ipchanger)
add_subdirectory(gui)
add_subdirectory(sandbox)