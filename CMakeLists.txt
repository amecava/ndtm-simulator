cmake_minimum_required(VERSION 2.8)
project("mtndsimulator" C)

# In-source build protection
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-std=c99")

add_subdirectory(src)
