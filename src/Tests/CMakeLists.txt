cmake_minimum_required(VERSION 3.13)

project(Tests)

set(CMAKE_C_STANDARD 11)

# Manually add CUnit include and library paths
include_directories(/usr/include/CUnit)
#link_directories(/usr/lib)

# Add the test file
add_executable(Tests main.c
		tests_utils.c
		tests_utils.h)

# Link the test executable with CUnit
target_link_libraries(Tests PRIVATE cunit)
