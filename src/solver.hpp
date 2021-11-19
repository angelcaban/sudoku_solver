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

#include <sudoku.hpp>

#define C \
  sudoku::cell {}

namespace sudoku {

struct sudoku_move {
  int move_num;
  int move_index;
};

sudoku_move no_move{.move_num = -1, .move_index = -1};

class Solver {
 private:
  int total_moves = 0;
  int max_moves;
  bool failure_break = false;

  sudoku_move find_next_move(Sudoku const& game, int row, int col,
                             const sudoku_move lastmove) {
    static std::random_device rng;

    auto cell = game.get(row, col);
    if (cell.is_invalid()) {
      return no_move;
    }

    auto next_index = lastmove.move_index + 1;
    if (next_index >= cell.count()) {
      return no_move;
    }

    auto digits = cell.possible_digits();
    return {
        .move_num = digits[next_index],
        .move_index = next_index,
    };
  }

  int random_coord() {
    static std::random_device rng;

    std::default_random_engine eng{rng()};
    std::uniform_int_distribution<int> dist{0, 8};

    return dist(eng);
  }

  bool hit_done_condition(Sudoku const& game, int row, int col) {
    bool bad_cell = false;
    if (row >= 0 && col >= 0) {
      bad_cell = game.get(row, col).is_invalid();
    }
    bool is_game_complete = game.is_complete();
    return failure_break || is_game_complete ||
           total_moves >= max_moves ||
           bad_cell;
  }

 public:
  Solver(int max) : max_moves(max) {
  }

  int get_moves() const {
    return total_moves;
  }

  bool is_failure() const {
    return failure_break;
  }

  bool solve(Sudoku& game) {
    // While we're not marked as done...
    while (!hit_done_condition(game, -1, -1)) {
      int randRow;
      int randCol;

      // Find the next coordinates with no distinct digit
      do {
        randRow = random_coord();
        randCol = random_coord();
        ++total_moves;
      } while (game.at(randRow, randCol).has_distinct() &&
               !hit_done_condition(game, randRow, randCol));

      sudoku_move my_move = no_move;
      recalculate_result recalc_result;

      do {
        // Find the next number in the list of possibilities for
        // the cell at (randRow, randCol)
        my_move = find_next_move(game, randRow, randCol, my_move);
        if (my_move.move_num == -1) {  // if no legal move
          // try to backtrack
          recalc_result = game.backtrack();
          if (recalc_result.curr_index == -1) {
            // break with failure if backtrack is impossible.
            return false;
          } else {
            // otherwise try again
            break;
          }
        }

        // If we have a good move, make the move...
        //   the game board is recalculated as part of make_move
        recalc_result = game.try_move(randRow, randCol, my_move.move_num);
      } while (recalc_result.resulted_in_bad_game);
    } // While not done

    return !failure_break;
  } // solve()

};

}  // namespace sudoku
