#include "finder.h"

#include <iostream>
#include <span>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/container/inlined_vector.h"
#include "absl/log/check.h"

namespace aoc2022 {

namespace {

std::vector<Type> ParseLine(std::string_view line) {
    std::vector<std::string_view> parts = absl::StrSplit(line, "");
    std::vector<Type> to_parts;
    to_parts.reserve(parts.size());
    for (std::string_view part : parts) {
        if (part == "#") {
            to_parts.push_back(Type::Blocked);
        } else if (part == ".") {
            to_parts.push_back(Type::Empty);
        } else if (part == "A") {
            to_parts.push_back(Type::A);
        } else if (part == "B") {
            to_parts.push_back(Type::B);
        } else if (part == "C") {
            to_parts.push_back(Type::C);
        } else if (part == "D") {
            to_parts.push_back(Type::D);
        } else {
            assert(false);
        }
    }
    return to_parts;
}

std::string TypeToString(const Type t) {
    switch (t) {
        case Type::Blocked:
            return "#";
        case Type::Empty:
            return ".";
        case Type::A:
            return "A";
        case Type::B:
            return "B";
        case Type::C:
            return "C";
        case Type::D:
            return "D";
    }
}

std::string Print(const std::vector<std::vector<Type>>& grid) {
    std::string ret;
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            absl::StrAppend(&ret, TypeToString(grid[i][j]));
        }
        absl::StrAppend(&ret, "\n");
    }
    return ret;
}

std::array<std::pair<int, int>, 4> ValidHomes(const Type t) {
    switch (t) {
        case Type::A:
            return kAHomes;
        case Type::B:
            return kBHomes;
        case Type::C:
            return kCHomes;
        case Type::D:
            return kDHomes;
        case Type::Empty:
        case Type::Blocked:
            assert(false);
            __builtin_unreachable();
    }
}

bool Complete(const std::vector<std::vector<Type>>& grid) {
    for (const std::pair<int, int>& a_pos : kAHomes) {
        if (grid[a_pos.first][a_pos.second] != Type::A) {
            return false;
        }
    }
    for (const std::pair<int, int>& b_pos : kBHomes) {
        if (grid[b_pos.first][b_pos.second] != Type::B) {
            return false;
        }
    }
    for (const std::pair<int, int>& c_pos : kCHomes) {
        if (grid[c_pos.first][c_pos.second] != Type::C) {
            return false;
        }
    }
    for (const std::pair<int, int>& d_pos : kDHomes) {
        if (grid[d_pos.first][d_pos.second] != Type::D) {
            return false;
        }
    }
    return true;
}

bool MovablePosition(const std::pair<int, int>&& pos, const Type type,
                     const std::vector<std::vector<Type>>& grid) {
    const std::array<std::pair<int, int>, 4> homes = ValidHomes(type);
    if (pos.first == homes[3].first && pos.second == homes[3].second) {
        return false;
    }

    // Track whether we're not at the correct home burrow.
    bool not_home = true;
    for (int home = 0; home < 3; ++home) {
        if (pos.first == homes[home].first &&
            pos.second == homes[home].second) {
            // We're at least at home.
            not_home = false;

            for (int below = home + 1; below < 4; ++below) {
                if (grid[homes[below].first][homes[below].second] != type) {
                    return true;
                }
            }
        }
    }
    return not_home;
}

std::vector<std::pair<int, int>> MovablePositions(
    const std::vector<std::vector<Type>>& grid) {
    std::vector<std::pair<int, int>> positions;
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            switch (grid[i][j]) {
                case Type::Empty:
                case Type::Blocked:
                    break;
                case Type::A: {
                    if (MovablePosition(std::make_pair(i, j), Type::A, grid)) {
                        positions.emplace_back(i, j);
                    }
                    break;
                }
                case Type::B: {
                    if (MovablePosition(std::make_pair(i, j), Type::B, grid)) {
                        positions.emplace_back(i, j);
                    }
                    break;
                }
                case Type::C: {
                    if (MovablePosition(std::make_pair(i, j), Type::C, grid)) {
                        positions.emplace_back(i, j);
                    }
                    break;
                }
                case Type::D: {
                    if (MovablePosition(std::make_pair(i, j), Type::D, grid)) {
                        positions.emplace_back(i, j);
                    }
                    break;
                }
            }
        }
    }
    return positions;
}

int Cost(const Type t) {
    switch (t) {
        case Type::A:
            return kAMoveCost;
        case Type::B:
            return kBMoveCost;
        case Type::C:
            return kCMoveCost;
        case Type::D:
            return kDMoveCost;
        default:
            assert(false);
            __builtin_unreachable();
    }
}

bool InHallway(const std::pair<int, int>& pos) {
    return pos.first == 1;
}

std::optional<std::pair<int, int>> OpenPathHome(
    const std::pair<int, int>& curr,
    const std::array<std::pair<int, int>, 4>& valid_homes,
    const std::vector<std::vector<Type>>& grid) {
    // If the higher of valid_homes is blocked, then backout early.
    if (grid[valid_homes[0].first][valid_homes[0].second] != Type::Empty) {
        return std::nullopt;
    }

    // If the lower of valid_homes is occupied by some other Type that doesn't
    // belong there, then backout.
    const Type current_type = grid[curr.first][curr.second];
    assert(current_type != Type::Empty && current_type != Type::Blocked);
    for (int i = 1; i < 4; ++i) {
        if (grid[valid_homes[i].first][valid_homes[i].second] != current_type &&
            grid[valid_homes[i].first][valid_homes[i].second] != Type::Empty) {
            return std::nullopt;
        }
    }

    // Compare columns to see if we're going left or right.
    const int curr_column = curr.second;
    const int curr_row = curr.first;
    const int target_column = valid_homes[0].second;
    bool leftwards = valid_homes[0].second < curr.second;

    if (leftwards) {
        // First walk to the column and see if anything blocks the way.
        for (int col = curr_column - 1; col >= target_column; --col) {
            if (grid[curr_row][col] != Type::Empty) {
                return std::nullopt;
            }
        }
    } else {
        // Right wards.
        for (int col = curr_column + 1; col <= target_column; ++col) {
            if (grid[curr_row][col] != Type::Empty) {
                return std::nullopt;
            }
        }
    }
    // We arrived at the column. Now go down. Take the lowest possible home.
    for (int i = 3; i >= 0; --i) {
        if (grid[valid_homes[i].first][valid_homes[i].second] == Type::Empty) {
            return valid_homes[i];
        }
    }
    CHECK(false);
}

int Cost(const std::pair<int, int>& destination,
         const std::pair<int, int>& curr,
         const std::vector<std::vector<Type>>& grid) {
    const int row_diff = std::abs(destination.first - curr.first);
    const int col_diff = std::abs(destination.second - curr.second);
    const Type t = grid[curr.first][curr.second];
    return Cost(t) * (row_diff + col_diff);
}

// Assumes `curr` is currently in a burrow.
absl::InlinedVector<std::pair<int, int>, 7> ValidNext(
    const std::pair<int, int>& curr,
    const std::vector<std::vector<Type>>& grid) {
    absl::InlinedVector<std::pair<int, int>, 7> ret;

    // If `curr` is in the lower burrow and the upper burrow contains a nonempty
    // type, then return empty vector.
    if (curr.first == 3 || curr.first == 4 || curr.first == 5) {
        for (int i = curr.first - 1; i >= 2; --i) {
            if (grid[i][curr.second] != Type::Empty) {
                return ret;
            }
        }
    }

    const Type curr_type = grid[curr.first][curr.second];
    const std::array<std::pair<int, int>, 4>& valid_homes =
        ValidHomes(curr_type);

    // Check for cases if its already in one of its valid homes.
    // We only need to be careful about the case when it's in one of the upper
    // floors.
    // If everything below is already filled with the correct type, then don't move it.
    if (curr.first == valid_homes[3].first &&
        curr.second == valid_homes[3].second) {
        return ret;
    }
    // Check if *all* of the lower floors is occupied by `curr_type`
    for (int i = 0; i < 3; ++i) {
        if (curr.first == valid_homes[i].first && curr.second == valid_homes[i].second) {
            bool all_occupied = true;
            for (int j = i + 1; j < 4; ++j) {
                if (grid[valid_homes[j].first][valid_homes[j].second] != curr_type) {
                    all_occupied = false;
                    break;
                }
            }
            if (all_occupied) {
                return ret;
            }
            break;
        }
    }

    // Not in its own home.
    // For each hallway coordinate insert if theres an open path.
    for (const std::pair<int, int>& hw : kHallways) {
        // Check if we can move.
        const bool leftwards = curr.second > hw.second;
        bool empty = true;
        if (leftwards) {
            for (int col = curr.second; col >= hw.second; --col) {
                if (grid[1][col] != Type::Empty) {
                    empty = false;
                    break;
                }
            }
        } else {
            for (int col = curr.second; col <= hw.second; ++col) {
                if (grid[1][col] != Type::Empty) {
                    empty = false;
                    break;
                }
            }
        }
        if (empty) {
            ret.push_back(hw);
        }
    }
    return ret;
}

}  // namespace

// Rules.
//  1. Amphipods will never stop above a hole. (kAboveHole)
//  2. Once an amphipod stops moving in the hallway, it will stay in that spot
//  until it can move into a room. (That is, once any amphipod starts moving,
//  any other amphipods currently in the hallway are locked in place and will
//  not move again until they can move fully into a room.)
//  3. Amphipods will never move from the hallway into a room unless that room
//  is their destination room and that room contains no amphipods which do not
//  also have that room as their own destination.
int Finder::FindMin() {
    return FindMinFromPosition(grid_);
}

int Finder::FindMinFromPosition(std::vector<std::vector<Type>>& grid) {
    if (Complete(grid)) {
        return 0;
    }
    int winning_min_cost = INT_MAX;
    for (const std::pair<int, int>& next : MovablePositions(grid)) {
        // Either move to hallway, or its in the hallway and we move it into its
        // spot.
        if (InHallway(next)) {
            std::array<std::pair<int, int>, 4> valid_homes =
                ValidHomes(grid[next.first][next.second]);
            const std::optional<std::pair<int, int>> can_go_home =
                OpenPathHome(next, valid_homes, grid);
            if (!can_go_home.has_value()) {
                continue;
            }
            const int cost = Cost(*can_go_home, next, grid);

            // Copy the grid and recurse.
            std::vector<std::vector<Type>> grid_cpy = grid;
            assert(grid_cpy[can_go_home->first][can_go_home->second] ==
                   Type::Empty);
            std::swap(grid_cpy[next.first][next.second],
                      grid_cpy[can_go_home->first][can_go_home->second]);
            auto it = grid_costs_.find(grid_cpy);
            const int recursive_min = it != grid_costs_.end()
                                          ? it->second
                                          : FindMinFromPosition(grid_cpy);
            if (it == grid_costs_.end()) {
                grid_costs_[grid_cpy] = recursive_min;
            }
            if (recursive_min == INT_MAX) {
                continue;
            }
            if (cost + recursive_min < winning_min_cost) {
                winning_min_cost = cost + recursive_min;
            }
            continue;
        }
        // Its in one of the burrows. Move to hallway.
        // Find all valid positions to move to recurse.
        absl::InlinedVector<std::pair<int, int>, 7> valid_next_positions =
            ValidNext(next, grid);
        for (const std::pair<int, int>& vnp : valid_next_positions) {
            const int cost = Cost(vnp, next, grid);
            std::vector<std::vector<Type>> grid_cpy = grid;
            assert(grid_cpy[vnp.first][vnp.second] == Type::Empty);
            std::swap(grid_cpy[next.first][next.second],
                      grid_cpy[vnp.first][vnp.second]);
            auto it = grid_costs_.find(grid_cpy);
            const int recursive_min = it != grid_costs_.end()
                                          ? it->second
                                          : FindMinFromPosition(grid_cpy);
            if (it == grid_costs_.end()) {
                grid_costs_[grid_cpy] = recursive_min;
            }
            if (recursive_min == INT_MAX) {
                continue;
            }
            if (cost + recursive_min < winning_min_cost) {
                winning_min_cost = cost + recursive_min;
            }
        }
    }
    return winning_min_cost;
}

Finder::Finder(std::span<const std::string> lines) {
    grid_.reserve(lines.size());
    for (std::string_view l : lines) {
        grid_.push_back(ParseLine(l));
    }
}

}  // namespace aoc2022