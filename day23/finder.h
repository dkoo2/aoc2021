#include <span>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"

namespace aoc2022 {

inline constexpr std::array<std::pair<int, int>, 4> kAHomes = {
    {{2, 3}, {3, 3}, {4, 3}, {5, 3}}};
inline constexpr std::array<std::pair<int, int>, 4> kBHomes = {
    {{2, 5}, {3, 5}, {4, 5}, {5, 5}}};
inline constexpr std::array<std::pair<int, int>, 4> kCHomes = {
    {{2, 7}, {3, 7}, {4, 7}, {5, 7}}};
inline constexpr std::array<std::pair<int, int>, 4> kDHomes = {
    {{2, 9}, {3, 9}, {4, 9}, {5, 9}}};

inline constexpr int kAMoveCost = 1;
inline constexpr int kBMoveCost = 10;
inline constexpr int kCMoveCost = 100;
inline constexpr int kDMoveCost = 1000;

inline constexpr std::array<std::pair<int, int>, 11> kHallways = {
    {{1, 1}, {1, 2}, {1, 4}, {1, 6}, {1, 8}, {1, 10}, {1, 11}}};

enum class Type {
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    Empty = 4,
    Blocked = 5,
};

class Finder {
   public:
    explicit Finder(std::span<const std::string> lines);
    int FindMin();

   private:
    int FindMinFromPosition(std::vector<std::vector<Type>>& grid);
    std::vector<std::vector<Type>> grid_;

    absl::flat_hash_map<std::vector<std::vector<Type>>, int> grid_costs_;
};

}  // namespace aoc2022
