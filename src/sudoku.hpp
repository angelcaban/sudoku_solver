
#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <map>
#include <utility>
#include <vector>

#include <cell.hpp>
#include <grid_iterator.hpp>
#include <span.hpp>
#include <history_node.hpp>
#include <grid_operations.hpp>

namespace sudoku {

struct recalculate_result {
    bool made_changes;
    bool resulted_in_bad_game;
    uint64_t curr_index;
};

/**
 * Abstraction for a Sudoku game.
 * Place numbers 1-9 in a 2-D 9-by-9 coordinate system.
 * Check placement against a small set of rules.
 */
class Sudoku {
public:
    Sudoku() : last_recalculate_result_{false, false, 0} {
        history_.emplace_back(std::move(grid{}), 0);
        recalculate_possibilities();
    }

    Sudoku(grid && grid) {
        history_.emplace_back(std::move(grid), 0);
        recalculate_possibilities();
    }

    recalculate_result get_last_recalculate_result() const {
        return last_recalculate_result_;
    }

    /** Go back a single point in time */
    recalculate_result backtrack() {
        recalculate_result result = {
            .made_changes = false,
            .resulted_in_bad_game = false,
            .curr_index = get_last_recalculate_result().curr_index,
        };

        if (history_.size() == 0 ||
            history_.at(last_recalculate_result_.curr_index).is_head()) {
            result.curr_index = -1;
            return result;
        }

        result.curr_index--;
        last_recalculate_result_ = result;
        return result;
    }

    /**
     * @brief Given the current state of the grid, reclaculate
     *        each cell's new possibility values.
     */
    recalculate_result recalculate_possibilities() {
        grid grid_copy{};
        auto & curr_node = history_.at(last_recalculate_result_.curr_index);
        std::copy(curr_node.get_grid().begin(), curr_node.get_grid().end(), grid_copy.begin());

        // Iterations happens in the original grid_ and
        // recalculated possibilities happen in grid_copy
        for (int i = 0; i < 9*9; ++i) { // (~7K steps)
            auto row = i / 9;
            auto col = i % 9;

            auto cellRow = grid_ops::get_row(row, data(), i);
            auto cellCol = grid_ops::get_column(col, data(), i);
            auto cellBox = grid_ops::get_inner_box(row, col, data(), i);

            auto remove_possibility = [&](auto c) {
                if (c.has_distinct()) {
                    grid_copy[i].remove_possibility(c.definite_number() - 0x30);
                }
            };

            std::for_each(cellRow.begin(), cellRow.end(), remove_possibility);
            std::for_each(cellCol.begin(), cellCol.end(), remove_possibility);
            std::for_each(cellBox.begin(), cellBox.end(), remove_possibility);

            remove_pair_collisions(cellRow, grid_copy[i]);
            remove_pair_collisions(cellCol, grid_copy[i]);
            remove_pair_collisions(cellBox, grid_copy[i]);

            reset_if_last_possible(cellRow, grid_copy[i]);
            reset_if_last_possible(cellCol, grid_copy[i]);
            reset_if_last_possible(cellBox, grid_copy[i]);
        }

        // ~200 Steps
        bool made_changes = curr_node.get_grid() != grid_copy;
        if (made_changes) {
            history_.emplace_back(std::move(grid_copy), history_.size());
        }

        last_recalculate_result_ = { 
            .made_changes = made_changes,
            .resulted_in_bad_game = is_invalid(),
            .curr_index = history_.size() - 1,
        };

        return last_recalculate_result_;
    }

    /** Make a move on the grid. This does not validate move against rules */
    recalculate_result make_move(int row, int col, int move) {
        at(row, col).reset_to(move);

        // Update possibilities for row, column & box
        bool made_changes = false;
        do {
            made_changes = recalculate_possibilities().made_changes;
        } while (made_changes);

        return last_recalculate_result_;
    }

    /** Grab a constant pointer to the underlying grid array */
    grid::const_pointer cdata() const {
        auto const& curr_node = history_.at(last_recalculate_result_.curr_index);
        return curr_node.cgrid().data();
    }

    /** Grab a pointer to the underlying grid array */
    grid::pointer data() {
        auto & curr_node = history_.at(last_recalculate_result_.curr_index);
        return curr_node.get_grid().data();
    }

    grid & current_grid() {
        auto & curr_node = history_.at(last_recalculate_result_.curr_index);
        return curr_node.get_grid();
    }

    grid const& current_grid() const {
        auto const& curr_node = history_.at(last_recalculate_result_.curr_index);
        return curr_node.cgrid();
    }

    /**
     * @brief Grab a mutable reference to a Cell in given row & column
     *
     * @param   row     A row index between 0 and 8
     * @param   col     A column index between 0 and 8
     */
    grid::reference at(int row, int col) {
        auto & curr_node = history_.at(last_recalculate_result_.curr_index);
        return curr_node.get_grid().at(grid_ops::calculate_index(row, col));
    }

    /**
     * @brief Grab an immutable copy of a Cell in a given row & column
     *
     * @param   row     A row index between 0 and 8
     * @param   col     A column index between 0 and 8
     */
    grid::value_type get(int row, int col) const {
        auto const& curr_node = history_.at(last_recalculate_result_.curr_index);
        return curr_node.cgrid().at(grid_ops::calculate_index(row, col));
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
        span row = grid_ops::get_row(r, data());
        span col = grid_ops::get_column(c, data());
        span box = grid_ops::get_inner_box(r, c, data());

        auto same_digit = [&](grid_iterator::value_type const& n) {
            return (n.count() == 1) && ((n.definite_number() - 0x30) == m);
        };

        return std::none_of(row.begin(), row.end(), same_digit)
            && std::none_of(col.begin(), col.end(), same_digit)
            && std::none_of(box.begin(), box.end(), same_digit);
    }

    /** Return true if all cells have a distinct possibility. */
    bool is_complete() const {
        return std::all_of(current_grid().cbegin(), current_grid().cend(), [](auto & c) {
            return c.count() == 1;
        });
    }

    /** Retrurn true if any cell has no possibility */
    bool is_invalid() const {
        return std::any_of(current_grid().cbegin(), current_grid().cend(), [](auto & c) {
            return c.count() == 0;
        });
    }

    friend std::ostream & operator<<(std::ostream & os, Sudoku & game) {
        os << "--------+-------+--------\n";
        for (int r = 0; r < 9; ++r) {
            os << "| ";
            for (int c = 0; c < 9; ++c) {
                bool third = ((c + 1) % 3) != 0;
                os << game.get(r, c) << ((third) ? " " : " | ");
            }
            os << '\n';
            if (((r + 1) % 3) == 0)
                os << "--------+-------+--------\n";
        }
        return os;
    }

private:
    std::vector<history_node> history_;
    recalculate_result last_recalculate_result_;

    bool no_pair_collision(grid_span where, int digit) {
        auto pair_counts = grid_ops::matching_pair_counter();
        auto only_two_possible = [](cell const& c) { return c.count() == 2; };
        auto onlytwos = std::find_if(where.begin(), where.end(), only_two_possible);;
        
        while (onlytwos != where.end()) {
            auto possible = onlytwos->get_possibilities();
            pair_counts.at(possible)++;

            if (pair_counts.at(possible) >= 2 && onlytwos->possibility_collides(digit)) {
                return false;
            }
            ++onlytwos;

            onlytwos = std::find_if(onlytwos, where.end(), only_two_possible);
        }

        return true;
    }

    void remove_pair_collisions(grid_span where, cell & c) {
        for (int i = 0; i < 9; ++i) {
            if (c.possibility_collides(i) && !no_pair_collision(where, i)) {
                c.remove_possibility(i);
            }
        }
    }

    void reset_if_last_possible(grid_span where, cell & c) {
        using std::placeholders::_1;

        auto is_colliding = [](cell &c, int i) { return c.possibility_collides(i); };
        for (int i = 0; i < 9; ++i) {
            if (c.possibility_collides(i) &&
                std::none_of(where.begin(), where.end(), std::bind(is_colliding, _1, i))) {
                    c.reset_to(i);
                    return;
            }
        }
    }
};

} // namespace sudoku 
