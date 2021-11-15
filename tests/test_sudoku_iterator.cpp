#define BOOST_TEST_MODULE Sudoku_Iterator

#include <boost/test/included/unit_test.hpp>

#include <sudoku.hpp>
#include <grid_operations.hpp>

BOOST_AUTO_TEST_CASE(test_get_row_0) {
    sudoku::Sudoku subject{
        { 1,2,3, 4,5,6, 7,8,9,
          4,5,6, 7,8,9, 1,2,3,
          7,8,9, 1,2,3, 4,5,6,
    //   ----------------------
          2,3,4, 5,6,7, 8,9,1,
          5,6,7, 8,9,1, 2,3,4,
          8,9,1, 2,3,4, 5,6,7,
    //   -----------------------
          3,4,5, 6,7,8, 9,1,2,
          6,7,8, 9,1,2, 3,4,5,
          9,1,2, 3,4,5, 6,7,8 }
    };

    auto s = sudoku::grid_ops::get_row(0, subject.data());
    auto begin = s.begin();
    auto end = s.end();

    auto val = *begin;
    BOOST_TEST(val.definite_number() == '1');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '2');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '3');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '4');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '5');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '6');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '7');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '8');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '9');
    ++begin;

    BOOST_TEST(begin == end);
}

BOOST_AUTO_TEST_CASE(test_get_row_5) {
    sudoku::Sudoku subject{
        { 1,2,3, 4,5,6, 7,8,9,
          4,5,6, 7,8,9, 1,2,3,
          7,8,9, 1,2,3, 4,5,6,
    //   ----------------------
          2,3,4, 5,6,7, 8,9,1,
          5,6,7, 8,9,1, 2,3,4,
          8,9,1, 2,3,4, 5,6,7,
    //   -----------------------
          3,4,5, 6,7,8, 9,1,2,
          6,7,8, 9,1,2, 3,4,5,
          9,1,2, 3,4,5, 6,7,8 }
    };

    auto s = sudoku::grid_ops::get_row(5, subject.data());
    auto begin = s.begin();
    auto end = s.end();
    
    auto val = *begin;
    BOOST_TEST(val.definite_number() == '8');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '9');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '1');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '2');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '3');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '4');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '5');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '6');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '7');
    ++begin;

    BOOST_TEST(begin == end);
}

BOOST_AUTO_TEST_CASE(test_get_col_0) {
    sudoku::Sudoku subject{
        { 1,2,3, 4,5,6, 7,8,9,
          4,5,6, 7,8,9, 1,2,3,
          7,8,9, 1,2,3, 4,5,6,
    //   ----------------------
          2,3,4, 5,6,7, 8,9,1,
          5,6,7, 8,9,1, 2,3,4,
          8,9,1, 2,3,4, 5,6,7,
    //   -----------------------
          3,4,5, 6,7,8, 9,1,2,
          6,7,8, 9,1,2, 3,4,5,
          9,1,2, 3,4,5, 6,7,8 }
    };

    auto s = sudoku::grid_ops::get_row(0, subject.cdata());
    auto begin = s.begin();
    auto end = s.end();

    auto val = *begin;
    BOOST_TEST(val.definite_number() == '1');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '2');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '3');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '4');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '5');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '6');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '7');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '8');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '9');
    ++begin;

    BOOST_TEST(begin == end);
}

BOOST_AUTO_TEST_CASE(test_get_col_8) {
    sudoku::Sudoku subject{
        { 1,2,3, 4,5,6, 7,8,9,
          4,5,6, 7,8,9, 1,2,3,
          7,8,9, 1,2,3, 4,5,6,
    //   ----------------------
          2,3,4, 5,6,7, 8,9,1,
          5,6,7, 8,9,1, 2,3,4,
          8,9,1, 2,3,4, 5,6,7,
    //   -----------------------
          3,4,5, 6,7,8, 9,1,2,
          6,7,8, 9,1,2, 3,4,5,
          9,1,2, 3,4,5, 6,7,8 }
    };

    auto s = sudoku::grid_ops::get_column(8, subject.data());
    auto begin = s.begin();
    auto end = s.end();

    auto val = *begin;
    BOOST_TEST(val.definite_number() == '9');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '3');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '6');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '1');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '4');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '7');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '2');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '5');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '8');
    ++begin;

    BOOST_TEST(begin == end);
}
BOOST_AUTO_TEST_CASE(test_get_inner_box_1_7) {
    sudoku::Sudoku subject{
        { 1,2,3, 4,5,6, 7,8,9,
          4,5,6, 7,8,9, 1,2,3,
          7,8,9, 1,2,3, 4,5,6,
    //   ----------------------
          2,3,4, 5,6,7, 8,9,1,
          5,6,7, 8,9,1, 2,3,4,
          8,9,1, 2,3,4, 5,6,7,
    //   -----------------------
          3,4,5, 6,7,8, 9,1,2,
          6,7,8, 9,1,2, 3,4,5,
          9,1,2, 3,4,5, 6,7,8 }
    };

    auto s = sudoku::grid_ops::get_inner_box(1, 7, subject.data());
    auto begin = s.begin();
    auto end = s.end();

    auto val = *begin;
    BOOST_TEST(val.definite_number() == '7');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '8');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '9');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '1');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '2');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '3');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '4');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '5');
    ++begin;

    val = *begin;
    BOOST_TEST(val.definite_number() == '6');
    ++begin;

    BOOST_TEST(begin == end);
}
