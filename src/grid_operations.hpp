#pragma once

#include <cell.hpp>
#include <grid_iterator.hpp>
#include <span.hpp>
#include <history_node.hpp>


bool operator==(sudoku::grid const& left, sudoku::grid const& right) {
    for (int i = 0; i < left.size(); ++i) {
        if (left[i] != right[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(sudoku::grid const& left, sudoku::grid const& right) {
    return !(left == right);
}

namespace sudoku {

class grid_ops {
public:

    static bool has_collisions(grid_span const& where) {
        std::array<int, 9> digit_count{0, 0, 0, 0, 0, 0, 0, 0, 0};
        auto add_distinct_collision = [&](cell const& c, int digit) {
            digit_count[digit - 1] +=
                (c.has_distinct() && c.possibility_collides(digit)) ? 1 : 0;
        };

        auto curr = where.cbegin();
        while (curr != where.cend()) {
            add_distinct_collision(*curr, 1);
            add_distinct_collision(*curr, 2);
            add_distinct_collision(*curr, 3);
            add_distinct_collision(*curr, 4);
            add_distinct_collision(*curr, 5);
            add_distinct_collision(*curr, 6);
            add_distinct_collision(*curr, 7);
            add_distinct_collision(*curr, 8);
            add_distinct_collision(*curr, 9);
            ++curr;
        }

        return std::any_of(digit_count.begin(), digit_count.end(), [](auto c) { return c > 1; });
    }

    static bool is_grid_invalid(grid const& g) {
        auto invalid = std::any_of(g.cbegin(), g.cend(), [](auto & c) {
            return c.is_invalid();
        });

        // Check for doubles in inner boxes
        if (!invalid) {
            invalid = invalid || has_collisions(grid_ops::get_inner_box(0, 0, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(3, 0, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(6, 0, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(0, 3, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(3, 3, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(6, 3, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(0, 6, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(3, 6, g.data()));
            invalid = invalid || has_collisions(grid_ops::get_inner_box(6, 6, g.data()));
        }

        // Check for doubles in rows & columns
        if (!invalid) {
            for (int i = 0; (i < 9) && (!invalid); ++i) {
                auto ithRow = grid_ops::get_row(i, g.data());
                auto ithCol = grid_ops::get_column(i, g.data());
                invalid = has_collisions(ithRow) || has_collisions(ithCol);
            }
        }

        return invalid;
    }

    /** Is the number of possibilities reduced to 2 */
    static bool only_two_possible(cell const& c) {
        return c.count() == 2;
    }

    /** Find cells that have only two possibilities */
    template<typename Iter>
    static grid_iterator find_only_twos(Iter begin, Iter end) {
        return std::find_if(begin, end, only_two_possible);
    }

    /** Return true if `digit` does not collide with pairs in `where` */
    static bool no_pair_collision(grid_span const& where, int digit) {
        auto pair_counts = grid_ops::matching_pair_counter();
        auto onlytwos = find_only_twos(where.cbegin(), where.cend());

        for (; onlytwos != where.cend();
                onlytwos = find_only_twos(onlytwos, where.cend())) {
            auto key = onlytwos->get_possibilities();
            pair_counts.at(key)++;

            if (pair_counts.at(key) >= 2 && onlytwos->possibility_collides(digit)) {
                return false;
            }
            ++onlytwos;
        }

        return true;
    }

    /** If cell `c` contains a possibility that no other cell in
     *  `where` has, then reset it to that digit */
    static bool reset_if_last_possible(grid_span where, cell & c) {
        using std::placeholders::_1;

        auto is_colliding = [](cell const& other_cell, int i) {
            return other_cell.possibility_collides(i);
        };
        for (int i = 1; i <= 9; ++i) {
            if (c.possibility_collides(i) &&
                std::none_of(where.begin(), where.end(), std::bind(is_colliding, _1, i))) {
                    c.reset_to(i);
                    return true;
            }
        }
        return false;
    }

    /** If there exists possible pairs in `where` and `c` has either of
     *  them as possibilities, then remove the possibility from `c` */
    static bool remove_pair_collisions(grid_span const& where, cell & c) {
        bool changed = false;
        for (int i = 0; i < 9; ++i) {
            if (c.possibility_collides(i) && !no_pair_collision(where, i)) {
                changed = true;
                c.remove_possibility(i);
            }
        }
        return changed;
    }

    /** Get a range that describes a given column, given `c` between 0 and 8 */
    static grid_span get_column(int c, grid::const_pointer ptr, grid::size_type except = -1) {
        auto s = calculate_index(0, c);
        indices indices = {
            index_or_skip(s, except),
            index_or_skip(s + 9, except),
            index_or_skip(s + 18, except),
            index_or_skip(s + 27, except),
            index_or_skip(s + 36, except),
            index_or_skip(s + 45, except),
            index_or_skip(s + 54, except),
            index_or_skip(s + 63, except),
            index_or_skip(s + 72, except)
        };
        return grid_span{
            grid_iterator{ptr, std::move(indices)},
            end_iter()
        };
    }

    /** Get a range that describes a given row, given `r` between 0 and 8 */
    static grid_span get_row(int r, grid::const_pointer ptr, grid::size_type except = -1) {
        auto s = calculate_index(r, 0);
        indices indices = {
            index_or_skip(s, except),
            index_or_skip(s + 1, except),
            index_or_skip(s + 2, except),
            index_or_skip(s + 3, except),
            index_or_skip(s + 4, except),
            index_or_skip(s + 5, except),
            index_or_skip(s + 6, except),
            index_or_skip(s + 7, except),
            index_or_skip(s + 8, except)
        };
        return grid_span{
            grid_iterator{ptr, std::move(indices)},
            end_iter()
        };
    }

    /** Get a range that describes a given inner box, given `r` & `c`
     *  are between 0 and 8 */
    static grid_span get_inner_box(int r, int c, grid::const_pointer ptr, grid::size_type except = -1) {
        int sRow = 3 * (r / 3);
        int sCol = 3 * (c / 3);
        auto s = calculate_index(sRow, sCol);
        indices indices = {
            index_or_skip(s, except),
            index_or_skip(s + 1, except),
            index_or_skip(s + 2, except),
            index_or_skip(s + 9, except),
            index_or_skip(s + 10, except),
            index_or_skip(s + 11, except),
            index_or_skip(s + 18, except),
            index_or_skip(s + 19, except),
            index_or_skip(s + 20, except)
        };
        return grid_span{
            grid_iterator{ptr, std::move(indices)},
            end_iter()
        };
    }

    /** Return i unless i == except, in which case return -1 */
    static constexpr grid::size_type index_or_skip(grid::size_type i, grid::size_type except) {
        return ((i == except) ? -1 : i);
    }

    /** Build a bogus iterator that represents an end to a grid */
    static grid_iterator end_iter() {
        return std::move(grid_iterator{nullptr, {9, 9, 9, 9, 9, 9, 9, 9, 9}, 9});
    }

    /** Remove possibility 'm' from all cells in a grid_span, 'gspan' */
    static void remove_possibility(grid_span && gspan, int m) {
        std::for_each(gspan.begin(), gspan.end(),
                        [=](auto c) { c.remove_possibility(m); });
    }

    /** Return true if no cells in the grid_span have digit, 'digit' */
    static bool no_single_digit_collision(grid_span && where, int digit) {
        auto same_digit = [&](grid_iterator::value_type const& n) {
            return (n.count() == 1) && ((n.definite_number() - 0x30) == digit);
        };
        return std::none_of(where.begin(), where.end(), same_digit);
    }

    /** Calculate a 1-D index from a 2-D coordinate */
    static constexpr sudoku::grid::size_type calculate_index(int row, int col) {
        return row * 9 + col;
    }

    /** Return a map of all cell possibility combinations for use in a counter */
    static std::map<std::uint_fast16_t, std::uint32_t> matching_pair_counter() {
        return std::map<std::uint_fast16_t, std::uint32_t>{
            { 0b000000011U, 0 },   // 1, 2
            { 0b000000101U, 0 },   // 1, 3
            { 0b000001001U, 0 },   // 1, 4
            { 0b000010001U, 0 },   // 1, 5
            { 0b000100001U, 0 },   // 1, 6
            { 0b001000001U, 0 },   // 1, 7
            { 0b010000001U, 0 },   // 1, 8
            { 0b100000001U, 0 },   // 1, 9
            { 0b000000110U, 0 },   // 2, 3
            { 0b000001010U, 0 },   // 2, 4
            { 0b000010010U, 0 },   // 2, 5
            { 0b000100010U, 0 },   // 2, 6
            { 0b001000010U, 0 },   // 2, 7
            { 0b010000010U, 0 },   // 2, 8
            { 0b100000010U, 0 },   // 2, 9
            { 0b000001100U, 0 },   // 3, 4
            { 0b000010100U, 0 },   // 3, 5
            { 0b000100100U, 0 },   // 3, 6
            { 0b001000100U, 0 },   // 3, 7
            { 0b010000100U, 0 },   // 3, 8
            { 0b100000100U, 0 },   // 3, 9
            { 0b000011000U, 0 },   // 4, 5
            { 0b000101000U, 0 },   // 4, 6
            { 0b001001000U, 0 },   // 4, 7
            { 0b010001000U, 0 },   // 4, 8
            { 0b100001000U, 0 },   // 4, 9
            { 0b000110000U, 0 },   // 5, 6
            { 0b001010000U, 0 },   // 5, 7
            { 0b010010000U, 0 },   // 5, 8
            { 0b100010000U, 0 },   // 5, 9
            { 0b001100000U, 0 },   // 6, 7
            { 0b010100000U, 0 },   // 6, 8
            { 0b100100000U, 0 },   // 6, 9
            { 0b011000000U, 0 },   // 7, 8
            { 0b101000000U, 0 },   // 7, 9
            { 0b110000000U, 0 },   // 8, 9
        };
    }
};


std::ostream & operator<<(std::ostream & os, sudoku::grid const& g) {
    os << "--------+-------+--------\n";
    for (int r = 0; r < 9; ++r) {
        os << "| ";
        for (int c = 0; c < 9; ++c) {
            bool third = ((c + 1) % 3) != 0;
            auto const& cell = g.at(grid_ops::calculate_index(r, c));
            os << cell << ((third) ? " " : " | ");
        }
        os << '\n';
        if (((r + 1) % 3) == 0)
            os << "--------+-------+--------\n";
    }
    return os;
}

} // namespace sudoku
