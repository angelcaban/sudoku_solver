#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

namespace sudoku {

using possibilities = std::uint32_t;

/**
 * A Sudoku cell is a single cell in a 9 by 9 grid that can
 * contain an number between 1 and 9.
 */
class cell {
public:
    /** Create an empty cell that can contain any possible digits 1 to 9, inclusive. */
    cell()
        : possibilities_{0x1FF},
          count_{9} {
    }

    /** Create a cell containing a single number */
    cell(int num)
        : possibilities_{0x0},
          count_{1} {
        num--;
        possibilities_ |= (1 << num);
    }

    /** Count the possibilities for this cell */
    int count() const {
        return count_;
    }

    /** Return a bitmask for cell possibilities */
    possibilities possibilities() const {
        return possibilities_;
    }

    /** Remove a possible number from this cell, where the number is between 1 and 9 */
    void removePossibility(int number) {
        number--;
        possibilities_ &= ~(1 << number);
    }

    /**
     * Return a printable character representing a digit between 1 and 9 IFF there is one possible number in this cell.
     * Return an 'X' if there are no possibilities for this cell.
     * Return a ' ' if there are multiple possibilities for this cell.
     */
    char definite_number() const {
        if (count_ == 0) return 'X';
        if (count_ > 1) return ' ';

        int i = 0;
        for (; (i < 10) && (0 == ((possibilities_ >> i) & 0x1)); ++i);

        return std::to_string(i+1)[0];
    }

    friend std::ostream & operator<<(std::ostream& os, cell cell) {
        for (int i = 0; i < 9; ++i) {
            if (1 == ((1 << i) & cell.possibilities_)) os << i + 1;
        }
    }

private:
    sudoku::possibilities possibilities_;
    int count_;
};

}
