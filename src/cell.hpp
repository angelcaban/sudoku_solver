#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace sudoku {

using possibilities = std::uint_fast16_t;

namespace priv {

template <std::size_t N>
constexpr std::array<int, N> add_possibles(possibilities p) {
  std::array<int, N> digits;
  int j = 0;
  for (int i = 0; i < N; ++i) {
    if (0 != (p & (1ULL << i))) digits[j++] = i + 1;
  }
  return std::move(digits);
}

// Helper structure to statically keep a mapping of bitmasks to digits
struct _constexpr_possibilities_map {
  constexpr char at(const uint64_t bitmap) const {
    if ((1ULL << 0ULL) == bitmap) return '1';
    if ((1ULL << 1ULL) == bitmap) return '2';
    if ((1ULL << 2ULL) == bitmap) return '3';
    if ((1ULL << 3ULL) == bitmap) return '4';
    if ((1ULL << 4ULL) == bitmap) return '5';
    if ((1ULL << 5ULL) == bitmap) return '6';
    if ((1ULL << 6ULL) == bitmap) return '7';
    if ((1ULL << 7ULL) == bitmap) return '8';
    if ((1ULL << 8ULL) == bitmap) return '9';
    return -1;
  }
};

}  // namespace priv

/**
 * A Sudoku cell is a single cell in a 9 by 9 get_grid that can
 * contain an number between 1 and 9.
 */
class cell {
 public:
  /** Create an empty cell that can contain any possible digits 1 to 9,
   * inclusive. */
  cell() : possibilities_{0x1FFULL}, count_{9} {}

  /** Create a cell containing a single number */
  cell(uint64_t num) : possibilities_{1ULL << (num - 1ULL)}, count_{1} {}

  possibilities get_possibilities() const { return possibilities_; }

  /** Return true if there's only a single number possible */
  bool has_distinct() const { return count_ == 1; }

  /** Return true if there are no more valid numbers for this cell */
  bool is_invalid() const { return count_ == 0; }

  /** Clear the possibilities and reset to empty state */
  void clear() {
    possibilities_ = 0x1FFULL;
    count_ = 9;
  }

  /** Count the possibilities for this cell */
  int count() const { return count_; }

  /** Reset this cell to have only one number */
  void reset_to(uint64_t num) {
    count_ = 1;
    possibilities_ = (1ULL << (num - 1ULL));
  }

  /** Remove a possible number from this cell, where the number is between 1 and
   * 9 */
  void remove_possibility(uint64_t number) {
    possibilities_ &= ~(1ULL << (number - 1ULL));
    // Re-count number of possibilities
    count_ = 0;
    count_ += (possibilities_)&0x01;
    count_ += (possibilities_ >> 1) & 0x01;
    count_ += (possibilities_ >> 2) & 0x01;
    count_ += (possibilities_ >> 3) & 0x01;
    count_ += (possibilities_ >> 4) & 0x01;
    count_ += (possibilities_ >> 5) & 0x01;
    count_ += (possibilities_ >> 6) & 0x01;
    count_ += (possibilities_ >> 7) & 0x01;
    count_ += (possibilities_ >> 8) & 0x01;
  }

  /** Return true if digit is one of this cell's possibilities, otherwise false
   */
  bool possibility_collides(int digit) const {
    return 0 != ((possibilities_ >> (digit - 1)) & 0x1);
  }

  /** Get an array of possible digits for this cell. */
  std::array<int, 9> possible_digits() const {
    return std::move(priv::add_possibles<9>(possibilities_));
  }

  /**
   * @brief Return a printable character representing a digit
   *        between 1 and 9 IFF there is one possible number in
   *        this cell.
   *        Return an 'X' if there are no possibilities for this
   *        cell.
   *        Return a ' ' if there are multiple possibilities for
   *        this cell.
   */
  char definite_number() const {
    if (is_invalid()) return 'X';
    if (!has_distinct()) return ' ';

    static constexpr auto bitfield_map = priv::_constexpr_possibilities_map{};

    return bitfield_map.at(possibilities_);
  }

  bool operator==(cell const& other) const {
    return this->possibilities_ == other.possibilities_;
  }

  bool operator!=(cell const& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& os, cell const& c) {
    os << c.definite_number();
    return os;
  }

 private:
  sudoku::possibilities possibilities_;
  int count_;
};

}  // namespace sudoku
