project(extrn)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE extrn_libs_sources
    ${MODULE_PATH}/*.c*
)

add_library(${PROJECT_NAME} ${extrn_libs_sources})

target_include_directories(${PROJECT_NAME}
    PUBLIC include
)
