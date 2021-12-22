cmake_minimum_required(VERSION 3.21.4)
project(hashCracker)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/hash_cracker)

file(GLOB hash_cracker_sources
    ${MODULE_PATH}/*.c*
)
message("Preparing ${files}")

add_executable(${PROJECT_NAME} ${hash_cracker_sources})
