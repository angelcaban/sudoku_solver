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

#define BOOST_TEST_MODULE Sudoku_Moves

#include <boost/test/included/unit_test.hpp>
#include <sudoku.hpp>

#define C \
  sudoku::cell {}

BOOST_AUTO_TEST_CASE(test_sudoku_at) {
  // clang-format off
  sudoku::Sudoku subject{{
    C,C,2, C,8,C, 6,4,C,
    5,C,C, 9,C,C, C,C,C,
    C,C,C, C,C,C, C,C,3,

    C,C,C, C,C,C, 1,C,C,
    C,C,6, C,4,C, 8,7,C,
    C,2,C, C,C,6, C,C,C,

    C,C,7, C,C,C, C,C,1,
    C,C,C, C,3,C, C,C,2,
    C,9,C, 8,C,C, 7,3,C
  }};
  // clang-format on

  auto cell = subject.at(4, 2);
  BOOST_TEST(cell.count() == 1);
  BOOST_TEST(cell.definite_number() == '6');

  cell = subject.at(2, 0);
  BOOST_TEST(cell.count() == 5);
  BOOST_TEST(cell.definite_number() == ' ');
}

BOOST_AUTO_TEST_CASE(test_sudoku_is_complete) {
  // clang-format off
  sudoku::Sudoku subject1{{
    C,C,2, C,8,C, 6,4,C,
    5,C,C, 9,C,C, C,C,C,
    C,C,C, C,C,C, C,C,3,

    C,C,C, C,C,C, 1,C,C,
    C,C,6, C,4,C, 8,7,C,
    C,2,C, C,C,6, C,C,C,

    C,C,7, C,C,C, C,C,1,
    C,C,C, C,3,C, C,C,2,
    C,9,C, 8,C,C, 7,3,C
  }};
  // clang-format on

  BOOST_TEST(false == subject1.is_complete());

  subject1.at(0, 0).remove_possibility(9);
  subject1.at(0, 0).remove_possibility(8);
  subject1.at(0, 0).remove_possibility(7);
  BOOST_TEST(false == subject1.is_complete());

  // clang-format off
  sudoku::Sudoku subject2{{
    1,2,3, 4,5,6, 7,8,9,
    4,5,6, 7,8,9, 1,2,3,
    7,8,9, 1,2,3, 4,5,6,

    2,3,4, 5,6,7, 8,9,1,
    5,6,7, 8,9,1, 2,3,4,
    8,9,1, 2,3,4, 5,6,7,

    3,4,5, 6,7,8, 9,1,2,
    6,7,8, 9,1,2, 3,4,5,
    9,1,2, 3,4,5, 6,7,8
  }};
  // clang-format on

  BOOST_TEST(true == subject2.is_complete());

  subject2.at(5, 5).clear();
  BOOST_TEST(false == subject2.is_complete());
}

#define test_distinct_cell(subject, row, col, val)         \
  BOOST_TEST(false == subject.at(row, col).is_invalid());  \
  BOOST_TEST(1 == subject.at(row, col).count());           \
  BOOST_TEST(true == subject.at(row, col).has_distinct()); \
  BOOST_TEST(val == subject.at(row, col).definite_number())

BOOST_AUTO_TEST_CASE(test_sudoku_recalculate_possibilities) {
  // clang-format off
  sudoku::Sudoku subject{{
    C,2,3, 4,5,6, 7,8,9,
    C,5,C, 7,C,9, 1,2,3,
    C,8,9, 1,2,3, 4,C,6,

    2,3,4, 5,6,7, 8,C,1,
    C,6,7, C,9,C, C,3,4,
    8,C,1, 2,3,4, 5,6,7,

    3,4,C, 6,7,8, 9,1,2,
    6,7,8, 9,1,C, 3,4,5,
    C,1,2, 3,4,5, 6,7,C
  }};
  // clang-format on

  sudoku::grid temp_grid;
  std::copy(subject.current_grid().cbegin(), subject.current_grid().cend(),
            temp_grid.begin());
  subject.recalculate_possibilities(std::move(temp_grid));

  test_distinct_cell(subject, 0, 0, '1');
  test_distinct_cell(subject, 0, 1, '2');
  test_distinct_cell(subject, 0, 2, '3');
  test_distinct_cell(subject, 0, 3, '4');
  test_distinct_cell(subject, 0, 4, '5');
  test_distinct_cell(subject, 0, 5, '6');
  test_distinct_cell(subject, 0, 6, '7');
  test_distinct_cell(subject, 0, 7, '8');
  test_distinct_cell(subject, 0, 8, '9');
}

BOOST_AUTO_TEST_CASE(test_sudoku_is_valid_move) {
  // clang-format off
  sudoku::Sudoku subject{{
    C,2,C, 4,5,6, 7,8,9,
    C,5,C, 7,C,9, C,2,C,
    C,8,9, C,2,C, 4,C,6,

    2,C,4, 5,6,7, 8,C,C,
    C,6,7, C,9,C, C,C,4,
    8,C,C, 2,C,4, 5,6,7,

    C,4,C, 6,7,8, 9,C,2,
    6,7,8, 9,C,C, C,4,5,
    C,C,2, C,4,5, 6,7,C
  }};
  // clang-format on

  BOOST_TEST(false == subject.is_valid_move(0, 0, 2));
  BOOST_TEST(true == subject.is_valid_move(0, 0, 1));
  BOOST_TEST(false == subject.is_valid_move(1, 4, 5));
  BOOST_TEST(true == subject.is_valid_move(1, 4, 8));
  BOOST_TEST(false == subject.is_valid_move(8, 8, 7));
  BOOST_TEST(false == subject.is_valid_move(8, 8, 9));
  BOOST_TEST(true == subject.is_valid_move(8, 8, 8));
}

BOOST_AUTO_TEST_CASE(test_sudoku_make_move) {
  // clang-format off
  auto subject = sudoku::Sudoku{{
    C,2,3, 4,5,6, 7,8,9,
    C,5,C, 7,C,9, 1,2,3,
    C,8,9, 1,2,3, 4,C,6,

    2,3,4, 5,6,7, 8,C,1,
    C,6,7, C,9,C, C,3,4,
    8,C,1, 2,3,4, 5,6,7,

    3,4,C, 6,7,8, 9,1,2,
    6,7,8, 9,1,C, 3,4,5,
    C,1,2, 3,4,5, 6,7,C
  }};
  // clang-format on
  subject.try_move(0, 0, 1);
  BOOST_TEST('1' == subject.at(0, 0).definite_number());
  BOOST_TEST('2' == subject.at(0, 1).definite_number());
  BOOST_TEST('3' == subject.at(0, 2).definite_number());
  BOOST_TEST('4' == subject.at(0, 3).definite_number());
  BOOST_TEST('5' == subject.at(0, 4).definite_number());
  BOOST_TEST('6' == subject.at(0, 5).definite_number());
}
