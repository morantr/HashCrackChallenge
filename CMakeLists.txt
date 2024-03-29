cmake_minimum_required(VERSION 3.21.4)
project(hashCracker)

# Update output directories for easy deployment
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True) # Do not fall back to a lower version

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -Wno-comment -Wno-unused-macros -Wno-parentheses \
  -Wno-unused-parameter -Wno-missing-field-initializers -Wno-deprecated-declarations \
  -Werror=format-security -Wpedantic -Wunused -Wshadow -Wfatal-errors -O3")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

option(BUILD_TESTS "build tests" ON)

if(BUILD_TESTS)
    message(STATUS "Tests enabled")

    # From google test quick start manual
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929 # Release GoogleTest 1.11.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_subdirectory(src)
