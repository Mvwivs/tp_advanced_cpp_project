
cmake_minimum_required(VERSION 3.10.2)

project(tp_advanced_cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_VERBOSE_MAKEFILE on)

add_subdirectory(src)
# add_subdirectory(test)
