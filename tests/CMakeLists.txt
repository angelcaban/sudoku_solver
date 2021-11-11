get_filename_component(PARENT_DIR ../src ABSOLUTE)

if (MSVC)
    add_compile_options(/std:c++latest)
else()
    add_compile_options(-std=gnu++20)
endif()

find_package(Boost REQUIRED)
include_directories(
    ${PARENT_DIR}
    ${Boost_INCLUDE_DIRS}
)

add_executable(test_sudoku_moves test_sudoku_moves.cpp)
add_executable(test_sudoku_iterator test_sudoku_iterator.cpp)

add_test(iterator_tests test_sudoku_iterator)
add_test(sudoku_moves test_sudoku_moves)
