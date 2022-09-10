/*
 * Sudoku Solver
 * Copyright (C) 2021-2022 Angel Caban
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
#include <boost/json/src.hpp>

#include <curl_wrapper.hpp>
#include <solver.hpp>

using namespace std;
using namespace sudoku;
namespace po = boost::program_options;

/**
 * Create a cell from an ASCII character.
 */
cell cell_from_char(char c) {
  auto num{ static_cast<uint64_t>(c) - 0x30 };
  return (c >= '1' && c <= '9') ? cell{static_cast<uint64_t>(num)} : C;
}

/**
 * Read a line of ASCII data in the form of:
 *   D,D,...
 * Where D is a single character.
 */
int read_line(string const& in_data, array<cell, 9*9> & user_grid, int next_i) {
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

/**
 * Read in ASCII data, assuming that all data is contained in a single line.
 */
int single_line_data(string const& in_data, array<cell, 9*9> & user_grid) {
  return read_line(in_data, user_grid, 0);
}

/**
 * Read in ASCII data, assuming that the data is split into multiple lines.
 */
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

/**
 * Read in data from the --data command line parameter.
 */
void read_data_param(const std::string& in_data, sudoku::grid& user_grid) {
  if (in_data[0] == '@') {
    ifstream instream{ in_data.substr(1) };
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
    cout << "Please provide 81 comma delimiated digits or spaces after the --data option.\n";
    exit(4);
  }
}

/**
 * Read in data from Sudoku.com
 */
void load_from_sudoku_dot_com(sudoku::grid& user_grid) {
  auto curl_wrapper = curl::CurlWrapper{};
  constexpr auto url{ "https://sudoku.com/api/level/evil" };

  if (curl::CurlError::SUCCESS != curl_wrapper.start_read(url)) {
    cout << "Unable to continue due to error in libcurl\n";
    exit(6);
  }

  boost::json::value val = boost::json::parse(curl_wrapper.stream().str());
  if (val.as_object().contains("mission")) {
    auto& mission = val.as_object()["mission"].as_string();
    cout << "Successfully read data from sudoku.com: " << mission << "\n";
    array<cell, 9 * 9>::size_type i = 0;
    for (char c : mission) {
      user_grid[i++] = cell_from_char(c);
    }
  }
  else {
    cout << "Malformed JSON. Expecting 'mission' field.\n";
    cout << boost::json::serialize(val) << '\n';
    exit(7);
  }
}

/**
 * Read in data from standard input.
 */
void read_stdin(sudoku::grid& user_grid) {
  cout << "Attempting to read from stdin...\n";
  if (file_data(cin, user_grid) != 81) {
    cout << "Data must contain 81 comma deliminated digits or spaces.\n";
    exit(5);
  }
}

/**
 * Parse in command-line options. Fill in the user_grid parameter if
 * parameters are valid.
 */
void parse_options(array<cell, 9*9> & user_grid, int argc, char ** argv) {
  po::options_description desc("Valid Options");
  desc.add_options()
    ("help", "Help Message")
    ("data", po::value<string>(), "Comma-separated list of 81 digits (1 to 9)"
                                  " or spaces, or @filename to read from a file")
    ("from-sudoku-com", "Grab a game from sudoku.com");
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << "sudoku_solver  Copyright (C) 2021-2022  Angel Caban\n"
            "This program comes with ABSOLUTELY NO WARRANTY.\n"
            "This is free software, and you are welcome to redistribute it\n"
            "under certain conditions. <https://www.gnu.org/licenses/gpl-3.0.en.html>\n\n";
    cout << desc << endl;
    exit(1);
  }

  if (vm.count("data")) {
    auto &in_data = vm["data"].as<string>();
    read_data_param(in_data, user_grid);
  }
  else if (vm.count("from-sudoku-com")) {
    auto& in_data = vm["from-sudoku-com"].as<string>();
    load_from_sudoku_dot_com(user_grid);
  }
  else {
    read_stdin(user_grid);
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
