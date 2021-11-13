#pragma once

#include <iostream>
#include <algorithm>
#include <ranges>

#include <iterator>

#include <span.hpp>
#include <cell.hpp>

namespace sudoku {

using grid_span = sudoku::span<class grid_iterator>;
using grid = std::array<cell, 9 * 9>;
using indices = std::array<grid::size_type, 9>;

/** An iterator for a Sudoku Grid */
class grid_iterator
#if defined(__GNUC__)
        : public std::iterator<std::forward_iterator_tag,
                               grid::value_type,
                               grid::difference_type,
                               grid::pointer,
                               grid::reference>
#endif
{
    static const uint64_t skip_index = static_cast<uint64_t>(-1);

public:
#if defined(_WIN32)
    using iterator_category = std::forward_iterator_tag;
    using value_type = grid::value_type;
    using difference_type = grid::difference_type;
    using pointer = grid::pointer;
    using reference = grid::reference;
#endif

    grid_iterator(pointer ptr, indices && indices)
        : ptr_{ptr}, indices_{std::move(indices)}, curr_idx_{0} {
    }

    grid_iterator(pointer ptr, indices && indices, int idx)
        : ptr_{ptr}, indices_{std::move(indices)}, curr_idx_{idx} {
    }

    pointer operator->() {
        // Skip over -1 values
        while (curr_idx_ < 9 && indices_[curr_idx_] == skip_index) { curr_idx_++; }
        return &(ptr_[indices_[curr_idx_]]);
    }

    reference operator*() {
        // Skip over -1 values
        while (curr_idx_ < 9 && indices_[curr_idx_] == skip_index) { curr_idx_++; }
        return ptr_[indices_[curr_idx_]];
    }

    grid_iterator& operator++() {
        // Skip over -1 values
        do { curr_idx_++; } while (curr_idx_ < 9 && indices_[curr_idx_] == skip_index);
        return *this;
    }

    friend bool operator==(grid_iterator const& a, grid_iterator const& b) {
        return a.curr_idx_ == b.curr_idx_;
    }

    friend bool operator!=(grid_iterator const& a, grid_iterator const& b) {
        return a.curr_idx_ != b.curr_idx_;
    }

    friend std::ostream & operator<<(std::ostream& os, grid_iterator const& iter) {
        os << "{ curr_idx_ = " << iter.curr_idx_ << " - ptr_ = " << iter.ptr_
           << " - indices = { ";
        std::copy(iter.indices_.begin(), iter.indices_.end(),
            std::ostream_iterator<grid::size_type>(os, ","));
        os << " } }";
        return os;
    }

private:
    pointer ptr_;
    indices indices_;
    int curr_idx_;
};

}

#if defined(__GNUC__)
template<>
std::iterator_traits<sudoku::grid_iterator>::iterator_category
std::__iterator_category(const sudoku::grid_iterator&) {
    return typename std::iterator_traits<sudoku::grid_iterator>::iterator_category();
}
#endif
