
#pragma once

#include <algorithm>
#include <array>

#include "cell.hpp"
#include "grid_iterator.hpp"
#include "span.hpp"

namespace sudoku {
namespace priv {

constexpr int calculate_index(int row, int col) {
    return row * 9 + col;
}

}

/**
 * Abstraction for a Sudoku game.
 * Place numbers 1-9 in a 2-D 9-by-9 coordinate system.
 * Check placement against a small set of rules.
 */
class Sudoku {
public:
    Sudoku() {
    }

    Sudoku(grid && grid) : grid_{std::move(grid)} {
    }

    /** Grab a constant pointer to the underlying grid array */
    grid::const_pointer data() const {
        return grid_.data();
    }

    /**
     * Grab a mutable reference to a Cell in given row & column
     *
     * @param   row     A row index between 0 and 8
     * @param   col     A column index between 0 and 8
     */
    grid::reference at(int row, int col) {
        return grid_[priv::calculate_index(row, col)];
    }

    /**
     * Grab an immutable copy of a Cell in a given row & column
     *
     * @param   row     A row index between 0 and 8
     * @param   col     A column index between 0 and 8
     */
    grid::value_type get(int row, int col) const {
        return grid_[priv::calculate_index(row, col)];
    }

    /**
     * Get a range that describes a given column
     * 
     * @param   c       A column index between 0 and 8
     */
    grid_span getColumn(int c) const {
        auto s = priv::calculate_index(0, c);
        indices indices = { s, s + 9, s + 18, s + 27, s + 36, s + 45, s + 54, s + 63, s + 72 };
        return grid_span{
            grid_iterator{data(), std::move(indices)},
            end_iter()
        };
    }

    /**
     * Get a range that describes a given row
     *
     * @param   r       A row index between 0 and 8
     */
    grid_span getRow(int r) const {
        auto s = priv::calculate_index(r, 0);
        indices indices = { s, s + 1, s + 2, s + 3, s + 4, s + 5, s + 6, s + 7, s + 8 };
        return grid_span{
            grid_iterator{data(), std::move(indices)},
            end_iter()
        };
    }

    /**
     * Get a range that describes a given inner box
     *
     * @param   r     A row index between 0 and 8
     * @param   c     A column index between 0 and 8
     */
    grid_span getInnerBox(int r, int c) const {
        int sRow = 3 * (r / 3);
        int sCol = 3 * (c / 3);
        auto s = priv::calculate_index(sRow, sCol);
        indices indices = {
                 s,  s + 1,  s + 2,
             s + 9, s + 10, s + 11,
            s + 18, s + 19, s + 20
        };
        return grid_span{
            grid_iterator{data(), std::move(indices)},
            end_iter()
        };
    }

    /**
     * Given a Sudoku grid coordinate and a number between 1 and 9,
     * determine if we can consider it a valid move
     *
     * @param   r   A row index between 0 and 8
     * @param   c   A column index between 0 and 8
     * @param   m   A number between 1 and 9
     * @return  `true` if m can be legally placed in (r, m), otherwise `false`
     */
    bool isValidMove(int r, int c, int m) const {
        span row = getRow(r);
        span col = getColumn(c);
        span box = getInnerBox(r, c);

        auto same_move = [&](int n) { return n == m; };

        return std::none_of(row.begin(), row.end(), same_move)
            && std::none_of(col.begin(), col.end(), same_move)
            && std::none_of(box.begin(), box.end(), same_move);
    }

    friend std::ostream & operator<<(std::ostream & os, Sudoku & game) {
        os << "--------+-------+--------\n";
        for (int r = 0; r < 9; ++r) {
            os << "| ";
            for (int c = 0; c < 9; ++c) {
                os << game.get(r, c) << (((c + 1) % 3) != 0) ? " " : " | ";
            }
            os << '\n';
            if (((r + 1) % 3) == 0)
                os << "--------+-------+--------\n";
        }
    }

private:
    grid grid_;

    static grid_iterator end_iter() {
        auto iter = grid_iterator{nullptr, {9, 9, 9, 9, 9, 9, 9, 9, 9}};
        return iter;
    }
};

}
