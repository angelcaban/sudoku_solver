#define BOOST_TEST_MODULE Sudoku_Full_Game

#include <boost/test/included/unit_test.hpp>

#include <random>
#include <sudoku.hpp>

#define C sudoku::cell{}

struct sudoku_move {
    int move_num;
    int move_index;
};

sudoku_move no_move{
    .move_num = -1,
    .move_index = -1
};

sudoku_move find_next_move(sudoku::cell const& cell, sudoku_move const& lastmove) {
    auto next_index = lastmove.move_index + 1;
    if (next_index >= cell.count()) { return no_move; }

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

BOOST_AUTO_TEST_CASE(test_sudoku) {
    using namespace sudoku;

    Sudoku game{{
      C,C,C, C,2,C, C,C,C,
      4,C,C, C,C,C, 1,C,C,
      C,3,C, 6,C,7, C,C,9,

      C,2,C, C,9,C, C,C,C,
      C,C,C, C,8,C, C,C,5,
      3,C,C, 2,C,5, C,4,C,

      C,6,C, 5,C,3, C,C,7,
      C,C,C, C,C,8, C,C,C,
      C,C,7, C,C,C, C,9,C
    }};

    std::array<char, 9*9> expected_grid{
       '6','8','9', '4','2','1', '5','7','3', 
       '4','7','5', '8','3','9', '1','6','2', 
       '2','3','1', '6','5','7', '4','8','9', 

       '5','2','8', '7','9','4', '6','3','1', 
       '7','1','4', '3','8','6', '9','2','5', 
       '3','9','6', '2','1','5', '7','4','8', 

       '9','6','2', '5','4','3', '8','1','7', 
       '1','4','3', '9','7','8', '2','5','6', 
       '8','5','7', '1','6','2', '3','9','4', 
    };

    auto hit_done_condition = [&](int row, int col) {
        bool bad_cell = false;
        if (row >= 0 && col >= 0) {
            bad_cell = game.at(row, col).is_invalid();
        }
        bool is_game_complete = game.is_complete();
        return is_game_complete || bad_cell;
    };

    while (!hit_done_condition(-1, -1)) {
        int randRow = random_coord();
        int randCol = random_coord();
        while (game.at(randRow, randCol).has_distinct()) {
          randRow = random_coord();
          randCol = random_coord();
        }

        auto const& cell = game.at(randRow, randCol);
        sudoku_move my_move = no_move;
        recalculate_result recalc_result;

        if (cell.is_invalid()) {
          recalc_result = game.backtrack();
          BOOST_TEST(recalc_result.curr_index == -1,
              "cell.invalid(): Unable to continue the game.");
          if (recalc_result.curr_index == -1) return;
        }

        do {
            my_move = find_next_move(cell, my_move);
            if (my_move.move_num == -1) {
                recalc_result = game.backtrack();
                BOOST_TEST(recalc_result.curr_index == -1,
                    "game.backtrack(): Unable to continue the game.");
                if (recalc_result.curr_index == -1) return;
            }

            recalc_result = game.try_move(randRow, randCol, my_move.move_num);
        } while (recalc_result.resulted_in_bad_game);
    }

    for (std::size_t i = 0; i < 9*9; ++i) {
      BOOST_TEST(expected_grid[i] == game.cdata()[i].definite_number());
    }
}

