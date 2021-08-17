#pragma once

#include <iterator>

#include "span.hpp"
#include "cell.hpp"

namespace sudoku {

using grid_span = sudoku::span<class grid_iterator>;
using grid = std::array<cell, 9 * 9>;
using indices = std::array<grid::size_type, 9>;

/** An iterator for a Sudoku Grid */
class grid_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = grid::value_type;
    using reference = grid::reference;
    using const_reference = grid::const_reference;
    using pointer = grid::pointer;
    using const_pointer = grid::const_pointer;

    grid_iterator(const_pointer ptr, indices && indices)
        : ptr_{ptr}, indices_{std::move(indices)}, curr_idx_{0} {
    }

    int current_index() const {
        return curr_idx_;
    }

    const value_type operator*() const {
        return ptr_[indices_[current_index()]];
    }

    grid_iterator& operator++() {
        curr_idx_++; return *this;
    }

    friend bool operator==(grid_iterator const& a, grid_iterator const& b) {
        return a.current_index() == b.current_index();
    }

    friend bool operator!=(grid_iterator const& a, grid_iterator const& b) {
        return a.current_index() != b.current_index();
    }

private:
    const_pointer ptr_;
    indices indices_;
    int curr_idx_;
};

}
