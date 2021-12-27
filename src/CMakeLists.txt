add_subdirectory("test")
add_subdirectory("external")

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB hash_cracker_sources
    ${MODULE_PATH}/*.c*
)
message(STATUS "Preparing ${hash_cracker_sources}")

add_executable(${PROJECT_NAME} ${hash_cracker_sources})

# Needs to be defined after 'add_executable()'
target_link_libraries(${PROJECT_NAME} extrn)
