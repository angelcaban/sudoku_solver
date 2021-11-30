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

#pragma once

#include <algorithm>
#include <array>
#include <cell.hpp>
#include <functional>
#include <grid_iterator.hpp>
#include <grid_operations.hpp>
#include <history_node.hpp>
#include <iterator>
#include <map>
#include <span.hpp>
#include <utility>
#include <vector>

namespace sudoku {

struct recalculate_result {
  bool made_changes;
  bool resulted_in_bad_game;
  uint64_t curr_index;
};

/**
 * Abstraction for a Sudoku game.
 * Place numbers 1-9 in a 2-D 9-by-9 coordinate system.
 * Check placement against a set of rules.
 */
class Sudoku {
 public:
  Sudoku() : last_recalculate_result_{false, false, 0} {
    history_.emplace_back(std::move(grid{}), 0, NO_INDEX);

    grid temp_grid;
    std::copy(current_grid().cbegin(), current_grid().cend(),
              temp_grid.begin());
    recalculate_possibilities(std::move(temp_grid));
  }

  Sudoku(grid &&g) : last_recalculate_result_{false, false, 0} {
    history_.emplace_back(std::move(g), 0, NO_INDEX);

    grid temp_grid;
    std::copy(current_grid().cbegin(), current_grid().cend(),
              temp_grid.begin());
    recalculate_possibilities(std::move(temp_grid));
  }

  recalculate_result get_last_recalculate_result() const {
    return last_recalculate_result_;
  }

  /** Go back a single point in time */
  recalculate_result backtrack() {
    recalculate_result result = {
        .made_changes = false,
        .resulted_in_bad_game = false,
        .curr_index = last_recalculate_result_.curr_index,
    };

    if (history_.size() == 0 ||
        history_.at(last_recalculate_result_.curr_index).is_head()) {
      result.curr_index = -1;
      return result;
    }

    auto parent =
        history_.at(last_recalculate_result_.curr_index).parent_index();
    if (parent == NO_INDEX)
      result.curr_index--;
    else
      result.curr_index = parent;

    last_recalculate_result_ = result;
    return result;
  }

  /**
   * @brief Given the current state of the grid, reclaculate
   *        each cell's new possibility values. Only update
   *        this Sudoku's grid if the move was valid.
   */
  recalculate_result recalculate_possibilities(grid &&temp_grid) {
    auto curr_idx = last_recalculate_result_.curr_index;

    do {
      last_recalculate_result_.made_changes = false;

      grid grid_copy;
      std::copy(temp_grid.cbegin(), temp_grid.cend(), grid_copy.begin());

      // Iterations happens in the original grid_ and
      // recalculated possibilities happen in grid_copy
      for (int i = 0; i < 9 * 9; ++i) {  // (~7K steps)
        auto row = i / 9;
        auto col = i % 9;

        auto cellRow = grid_ops::get_row(row, temp_grid.data(), i);
        auto cellCol = grid_ops::get_column(col, temp_grid.data(), i);
        auto cellBox = grid_ops::get_inner_box(row, col, temp_grid.data(), i);

        auto remove_possibility = [&](auto &c) {
          if (c.has_distinct()) {
            grid_copy[i].remove_possibility(c.definite_number() - 0x30);
          }
        };

        std::for_each(cellRow.begin(), cellRow.end(), remove_possibility);
        std::for_each(cellCol.begin(), cellCol.end(), remove_possibility);
        std::for_each(cellBox.begin(), cellBox.end(), remove_possibility);

        grid_ops::remove_pair_collisions(cellRow, grid_copy[i]);
        grid_ops::remove_pair_collisions(cellCol, grid_copy[i]);
        grid_ops::remove_pair_collisions(cellBox, grid_copy[i]);

        grid_ops::reset_if_last_possible(cellRow, grid_copy[i]);
        grid_ops::reset_if_last_possible(cellCol, grid_copy[i]);
        grid_ops::reset_if_last_possible(cellBox, grid_copy[i]);
      }

      if (temp_grid != grid_copy) {
        last_recalculate_result_.made_changes = true;
        if (grid_ops::is_grid_invalid(grid_copy)) {
          // Don't try pushing back the results if this move was invalid
          last_recalculate_result_.resulted_in_bad_game = true;
          return last_recalculate_result_;
        }
        temp_grid = std::move(grid_copy);
      }
    } while (last_recalculate_result_.made_changes);

    if (current_grid() != temp_grid) {
      auto next_index = history_.size();
      auto &new_node =
          history_.emplace_back(std::move(temp_grid), next_index, curr_idx);
      history_.at(curr_idx).add_branch(next_index);

      last_recalculate_result_ = {
          .made_changes = true,
          .resulted_in_bad_game = is_invalid(),
          .curr_index = next_index,
      };
    }

    return last_recalculate_result_;
  }

  /**
   * @brief Try to make a move on the grid. This method calculates whether
   *        or not the move is valid and returns a structure with the result.
   *        If the move is valid, it is stored as part of the move history.
   */
  recalculate_result try_move(int row, int col, int move) {
    grid temp_grid;
    std::copy(current_grid().cbegin(), current_grid().cend(),
              temp_grid.begin());
    temp_grid.at(grid_ops::calculate_index(row, col)).reset_to(move);

    // Update possibilities for row, column & box

    return recalculate_possibilities(std::move(temp_grid));
  }

  /** Grab the current grid in the history vector */
  grid &current_grid() {
    auto &curr_node = history_.at(last_recalculate_result_.curr_index);
    return curr_node.get_grid();
  }

  /** Grab the current grid in the history vector */
  grid const &current_grid() const {
    auto const &curr_node = history_.at(last_recalculate_result_.curr_index);
    return curr_node.cgrid();
  }

  /** Grab a constant pointer to the underlying grid array */
  grid::const_pointer cdata() const {
    auto const &curr_node = history_.at(last_recalculate_result_.curr_index);
    return current_grid().data();
  }

  /** Grab a pointer to the underlying grid array */
  grid::pointer data() {
    auto &curr_node = history_.at(last_recalculate_result_.curr_index);
    return current_grid().data();
  }

  /**
   * @brief Grab a mutable reference to a Cell in given row & column
   *
   * @param   row     A row index between 0 and 8
   * @param   col     A column index between 0 and 8
   */
  grid::reference at(int row, int col) {
    return current_grid().at(grid_ops::calculate_index(row, col));
  }

  /**
   * @brief Grab an immutable copy of a Cell in a given row & column
   *
   * @param   row     A row index between 0 and 8
   * @param   col     A column index between 0 and 8
   */
  grid::value_type get(int row, int col) const {
    return current_grid().at(grid_ops::calculate_index(row, col));
  }

  /**
   * @brief Given a Sudoku grid coordinate and a number between 1 and 9,
   *        determine if we can consider it a valid move
   *
   * @param   r   A row index between 0 and 8
   * @param   c   A column index between 0 and 8
   * @param   m   A number between 1 and 9
   * @return  `true` if m can be legally placed in (r, m), otherwise `false`
   */
  bool is_valid_move(int r, int c, int m) {
    auto except = r * 9 + c;
    auto row = grid_ops::get_row(r, data(), except);
    auto col = grid_ops::get_column(c, data(), except);
    auto box = grid_ops::get_inner_box(r, c, data(), except);

    auto same_digit = [&](grid_iterator::value_type const &n) {
      return (n.count() == 1) && ((n.definite_number() - 0x30) == m);
    };

    return std::none_of(row.begin(), row.end(), same_digit) &&
           std::none_of(col.begin(), col.end(), same_digit) &&
           std::none_of(box.begin(), box.end(), same_digit);
  }

  /** Return true if all cells have a distinct possibility. */
  bool is_complete() const {
    return std::all_of(current_grid().cbegin(), current_grid().cend(),
                       [](auto &c) { return c.count() == 1; });
  }

  /** Retrurn true if any cell has no possibility */
  bool is_invalid() const { return grid_ops::is_grid_invalid(current_grid()); }

  friend std::ostream &operator<<(std::ostream &os, Sudoku const &game) {
    os << game.current_grid();
    return os;
  }

 private:
  std::vector<history_node> history_;
  recalculate_result last_recalculate_result_;
};

}  // namespace sudoku
