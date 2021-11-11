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

    /**
     * @brief Get a range that describes a given column
     * 
     * @param   c       A column index between 0 and 8
     */
    static grid_span get_column(int c, grid::pointer ptr, grid::size_type except = -1) {
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

    /**
     * @brief Get a range that describes a given row
     *
     * @param   r       A row index between 0 and 8
     */
    static grid_span get_row(int r, grid::pointer ptr, grid::size_type except = -1) {
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

    /**
     * @brief Get a range that describes a given inner box
     *
     * @param   r     A row index between 0 and 8
     * @param   c     A column index between 0 and 8
     */
    static grid_span get_inner_box(int r, int c, grid::pointer ptr, grid::size_type except = -1) {
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

    static constexpr grid::size_type index_or_skip(grid::size_type i, grid::size_type except) {
        return ((i == except) ? -1 : i);
    }

    static grid_iterator end_iter() {
        return std::move(grid_iterator{nullptr, {9, 9, 9, 9, 9, 9, 9, 9, 9}, 9});
    }

    static void remove_possibility(grid_span && gspan, int m) {
        std::for_each(gspan.begin(), gspan.end(),
                        [=](auto c) { c.remove_possibility(m); });
    }

    static bool no_single_digit_collision(grid_span && where, int digit) {
        auto same_digit = [&](grid_iterator::value_type const& n) {
            return (n.count() == 1) && ((n.definite_number() - 0x30) == digit);
        };
        return std::none_of(where.begin(), where.end(), same_digit);
    }

    static constexpr sudoku::grid::size_type calculate_index(int row, int col) {
        return row * 9 + col;
    }

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

} // namespace sudoku
