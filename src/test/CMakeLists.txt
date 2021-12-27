if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # Release GoogleTest 1.11.0
    )
    FetchContent_MakeAvailable(googletest)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_executable(unit_test
        unit.cpp
    )
    target_link_libraries(unit_test gtest_main extrn)
    add_test(NAME unit_test COMMAND unit)

endif()
