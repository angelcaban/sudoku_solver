#pragma once

#include <vector>

#include <grid_iterator.hpp>

namespace sudoku {

class history_node {
    std::vector<history_node *> next_branches_;
    grid this_grid_;

    std::size_t this_grid_index_ = -1;
    std::size_t prev_grid_index_ = -1;

public:

    history_node() {
    }

    history_node(grid && this_grid, std::size_t this_index)
        : this_grid_{std::move(this_grid)}, this_grid_index_{this_index} {
    }

    grid const& cgrid() const {
        return this_grid_;
    }

    grid & get_grid() {
        return this_grid_;
    }

    /** The location for this node's parent in the history vector */
    std::size_t previous_index() const {
        return prev_grid_index_;
    }

    /** The locatino of this node in the history vector */
    std::size_t index() const {
        return this_grid_index_;
    }

    bool is_head() const {
        return prev_grid_index_ == -1;
    }

    bool is_leaf() const {
        return next_branches_.size() == 0;
    }

    bool has_branches() const {
        return next_branches_.size() > 1;
    }

    void add_branch(history_node * next) {
        next->prev_grid_index_ = this_grid_index_;
        next_branches_.push_back(next);
    }
};
   
} // namespace sudoku
