/*
 * Sudoku Solver
 * Copyright (C) 2021 Angel Caban
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
