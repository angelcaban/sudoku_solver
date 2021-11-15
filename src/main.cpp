#include <chrono>
#include <iostream>
#include <random>
#include <sudoku.hpp>

using namespace std;
using namespace sudoku;

struct sudoku_move {
  int move_num;
  int move_index;
};

sudoku_move no_move{.move_num = -1, .move_index = -1};

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

#define C \
  sudoku::cell {}

int main(int argc, char** argv) {
  /*
      Sample sudoku board

      grid sample{
          4,3,5, 2,6,9, 7,8,1,
          6,8,2, 5,7,1, 4,9,3,
          1,9,7, 8,3,4, 5,6,2,

          8,2,6, 1,9,5, 3,4,7,
          3,7,4, 6,8,2, 9,1,5,
          9,5,1, 7,4,3, 6,2,8,

          5,1,9, 3,2,6, 8,7,4,
          2,4,8, 9,5,7, 1,3,6,
          7,6,3, 4,1,8, 2,5,9};
      Sudoku game{std::move(sample)};

      cout << game << endl; */

  const auto max_moves = 1000;
  bool failure_break = false;
  auto timerstart = std::chrono::high_resolution_clock::now();

  // clang-format off
  // Empty sudoku board
  Sudoku game{{
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

  // Make an initial random move to start off
  // auto randX = random_coord();
  // auto randY = random_coord();
  // auto randMove = random_coord() + 1;
  // game.at(randX, randY).reset_to(randMove);

  // Keep track of the total number of moves.
  int totalMoves = 0;

  // We're done on one of three scenarios:
  //   1) The game is completed successfully
  //   2) The total number of moves exceed the maximum allowed
  //   3) The last cell has exhausted all possibilities
  auto hit_done_condition = [&](int row, int col) {
    bool bad_cell = false;
    if (row >= 0 && col >= 0) {
      bad_cell = game.at(row, col).is_invalid();
    }
    bool is_game_complete = game.is_complete();
    return failure_break || is_game_complete || totalMoves >= max_moves ||
           bad_cell;
  };

  // While we're not marked as done...
  while (!hit_done_condition(-1, -1)) {
    int randRow;
    int randCol;

    // Find the next coordinates with no distinct digit
    do {
      randRow = random_coord();
      randCol = random_coord();
      ++totalMoves;
    } while (game.at(randRow, randCol).has_distinct() &&
             !hit_done_condition(randRow, randCol));

    sudoku_move my_move = no_move;
    recalculate_result recalc_result;

    // While there are still legal moves...
    do {
      // Find the next option in the list of possibilities for
      // the cell at (randRow, randCol)
      my_move = find_next_move(game, randRow, randCol, my_move);
      if (my_move.move_num == -1) {  // if no legal move
        // try to backtrack
        recalc_result = game.backtrack();
        if (recalc_result.curr_index == -1) {
          // break with failure if backtrack is impossible.
          failure_break = true;
          break;
        } else {
          // otherwise try again
          break;
        }
      }

      // If we have a good move, make the move...
      //   the game board is recalculated as part of make_move
      recalc_result = game.try_move(randRow, randCol, my_move.move_num);
    } while (recalc_result.resulted_in_bad_game);

    if (failure_break) {
      cout << "Breaking away due to a failure." << endl;
      break;
    };

    // cout << "Move #" << totalMoves << " put '" << my_move.move_num
    //      << "' into (" << randRow+1 << "," << randCol+1 << ")\n";
    // cout << game << endl;
  }
  auto timerend = std::chrono::high_resolution_clock::now();

  cout << game << endl;
  if (!game.is_complete())
    cout << "Unable to find a solution after ";
  else
    cout << "Able to find a solution after ";
  std::chrono::duration<double> elapsed = timerend - timerstart;
  cout << elapsed.count() << "s and " << totalMoves << " moves" << endl;

  return 0;
}
