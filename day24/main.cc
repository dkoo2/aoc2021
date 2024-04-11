#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/str_cat.h"
#include "parser.h"

// Program entry point.
// Reads infile and parses the text.
int main() {
    std::ifstream input;
    const std::string filepath = absl::StrCat(
        std::filesystem::current_path().string(), "/", "infile.txt");
    input.open(filepath);
    assert(input.is_open());
    std::vector<std::string> strings;
    std::string line;
    while (input) {
        getline(input, line);
        strings.push_back(line);
    }
    input.close();
    aoc2022::Parser parser(strings);
    std::cout << parser.ParallelFinder() << std::endl;

    return 0;
}
