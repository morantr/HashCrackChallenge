cmake_minimum_required(VERSION 3.21.4)
project(hashCracker)

# Update output directories for easy deployment
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True) # Do not fall back to C++98

add_subdirectory(src)
