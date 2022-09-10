cmake_minimum_required(VERSION 3.10)

project(sudoku VERSION 0.1)
if (MSVC)
    add_compile_options(/std:c++latest)
else()
    add_compile_options(-std=gnu++20)
endif()

get_filename_component(SRC_DIR ./src ABSOLUTE)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(CURL)
include_directories(
    ${SRC_DIR}
    ${Boost_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)

enable_testing()

add_subdirectory(tests)

set(CMAKE_SOURCE_DIR src)

add_executable(sudoku src/main.cpp)
target_link_libraries(sudoku ${Boost_LIBRARIES} ${CURL_LIBRARIES})
