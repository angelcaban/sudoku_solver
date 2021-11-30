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

#include <algorithm>
#include <cctype>
#include <chrono>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

#include <boost/algorithm/string.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <solver.hpp>

using namespace std;
using namespace sudoku;
namespace po = boost::program_options;

cell cell_from_char(char c) {
  return (c >= '1' && c <= '9') ? cell{static_cast<uint64_t>(c - 0x30)} : C;
}

int read_line(string& in_data, array<cell, 9*9> & user_grid, int next_i) {
  char lastc = '\0';
  for (auto && c : in_data) {
    if (c == ',') {
      user_grid[next_i++] = cell_from_char(lastc);
      lastc = '\0';
    } else {
      lastc = c;
    }
  }
  if (in_data.back() != ',')
    user_grid[next_i++] = cell_from_char(lastc);
  return next_i;
}

int single_line_data(string& in_data, array<cell, 9*9> & user_grid) {
  return read_line(in_data, user_grid, 0);
}

int file_data(istream& instream, array<cell, 9*9> & user_grid) {
  string line;
  int i = 0;
  while (getline(instream, line)) {
    line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
    if (line.empty()) continue;
    i = read_line(line, user_grid, i);
  }
  return i;
}

void parse_options(array<cell, 9*9> & user_grid, int argc, char ** argv) {
  po::options_description desc("Valid Options");
  desc.add_options()
    ("help", "Help Message")
    ("data", po::value<string>(), "Comma-separated list of 81 digits (1 to 9) or spaces, or @filename to read from a file");
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << "sudoku_solver  Copyright (C) 2021  Angel Caban\n"
            "This program comes with ABSOLUTELY NO WARRANT.\n"
            "This is free software, and you are welcome to redistribute it\n"
            "under certain conditions. <https://www.gnu.org/licenses/gpl-3.0.en.html>\n\n";
    cout << desc << "\n";
    exit(1);
  }

  if (vm.count("data")) {
    auto in_data = vm["data"].as<string>();
    if (in_data[0] == '@') {
      ifstream instream{in_data.substr(1)};
      if (!instream.is_open()) {
        cout << "Unable to open '" << in_data.substr(1) << "'\n";
        exit(3);
      }
      if (file_data(instream, user_grid) != 81) {
        cout << "File must contain 81 comma deliminated digits or spaces.\n";
        exit(2);
      }
    }
    else if (single_line_data(in_data, user_grid) != 81) {
      cout << "Please provide 81 comma delimiated digits or spaces after the --data option\n";
      exit(4);
    }
  }
  else {
    cout << "Attempting to read from stdin...\n";
    if (file_data(cin, user_grid) != 81) {
      cout << "Data must contain 81 comma deliminated digits or spaces.\n";
      exit(5);
    }
  }
}

int main(int argc, char** argv) {
  const auto max_moves = 1000;
  array<cell, 9*9> user_grid;
  parse_options(user_grid, argc, argv);

  auto timerstart = std::chrono::high_resolution_clock::now();

  Sudoku game{std::move(user_grid)};
  Solver my_solver{max_moves};

  auto is_success = my_solver.solve(game);
  auto timerend = std::chrono::high_resolution_clock::now();

  cout << game << endl;
  if (is_success)
    cout << "Able to find a solution after ";
  else
    cout << "Unable to find a solution after ";

  std::chrono::duration<double> elapsed = timerend - timerstart;
  cout << elapsed.count() << "s and " << my_solver.get_moves() << " moves" << endl;

  return 0;
}
