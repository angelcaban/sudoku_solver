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

namespace sudoku {

/** Container for an iterator range */
template <typename T>
class span {
 public:
  span() {}

  span(T&& begin, T&& end) : begin_{std::move(begin)}, end_{std::move(end)} {}

  T& begin() { return begin_; }

  T& end() { return end_; }

  T const& cbegin() const { return begin_; }

  T const& cend() const { return end_; }

 private:
  T begin_;
  T end_;
};

}  // namespace sudoku
