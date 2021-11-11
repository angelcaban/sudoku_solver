cmake_minimum_required(VERSION 3.10)

project(sudoku VERSION 0.1)
if (MSVC)
    add_compile_options(/std:c++latest)
else()
    add_compile_options(-std=gnu++20)
endif()

enable_testing()

get_filename_component(SRC_DIR ./src ABSOLUTE)

add_subdirectory(tests)

set(CMAKE_SOURCE_DIR src)

find_package(Boost REQUIRED)

include_directories(
    ${SRC_DIR}
    ${Boost_INCLUDE_DIRS}
)

add_executable(sudoku src/main.cpp)
