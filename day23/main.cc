#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/str_cat.h"
#include "finder.h"

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
    aoc2022::Finder finder(strings);
    std::cout << finder.FindMin() << std::endl;

    return 0;
}
