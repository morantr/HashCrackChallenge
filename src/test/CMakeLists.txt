add_executable(unit_test
    unit.cpp
    ../BaseOperationsUtils.cpp
    ../UiUtils.cpp
    ../HashGenerator.cpp
)
target_link_libraries(unit_test gtest_main extrn)
