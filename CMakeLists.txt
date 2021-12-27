cmake_minimum_required(VERSION 3.21.4)
project(hashCracker)

# Update output directories for easy deployment
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/hash_cracker)

add_subdirectory(src)


