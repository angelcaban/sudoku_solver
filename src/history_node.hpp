/*
 * Sudoku Solver
 * Copyright (C) 2021 Angel Caban
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <climits>
#include <grid_iterator.hpp>
#include <limits>
#include <vector>

namespace sudoku {

constexpr std::size_t NO_INDEX = ULLONG_MAX;

class history_node {
  std::vector<std::size_t> next_branches_;
  grid this_grid_;

  std::size_t this_grid_index_;
  std::size_t prev_grid_index_;

 public:
  history_node() : this_grid_index_{NO_INDEX}, prev_grid_index_{NO_INDEX} {}

  history_node(grid&& this_grid, std::size_t this_index,
               std::size_t parent_index)
      : this_grid_{std::move(this_grid)},
        this_grid_index_{this_index},
        prev_grid_index_{parent_index} {}

  grid const& cgrid() const { return this_grid_; }

  grid& get_grid() { return this_grid_; }

  void overwrite(grid&& g) { this_grid_ = std::move(g); }

  /** The location for this node's parent in the history vector */
  std::size_t parent_index() const { return prev_grid_index_; }

  /** The locatino of this node in the history vector */
  std::size_t index() const { return this_grid_index_; }

  bool is_head() const { return prev_grid_index_ == NO_INDEX; }

  bool is_leaf() const { return next_branches_.size() == 0; }

  bool has_branches() const { return next_branches_.size() > 1; }

  void add_branch(std::size_t child_index) {
    next_branches_.push_back(child_index);
  }
};

}  // namespace sudoku
