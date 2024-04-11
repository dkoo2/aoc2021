#include "parser.h"

#include <iostream>
#include <span>
#include <string>
#include <vector>

#include "absl/container/inlined_vector.h"
#include "absl/log/check.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/synchronization/blocking_counter.h"
#include "absl/types/span.h"

namespace aoc2022 {

SingleProgram::SingleProgram(absl::Span<const std::string> strings) {
    instructions_.reserve(strings.size());
    for (const std::string& s : strings) {
        instructions_.emplace_back(s);
    }
}

std::string SingleProgram::DebugPrint() const {
    std::string ret;
    for (const Instruction& instruction : instructions_) {
        absl::StrAppend(&ret, instruction.Print(), " ");
    }
    absl::StrAppend(&ret, "\n");
    return ret;
}

namespace {

int64_t VarsFromCurrentStatus(const Vars v, const int64_t x, const int64_t y,
                              const int64_t z, const int64_t w) {
    switch (v) {
        case Vars::kX:
            return x;
        case Vars::kY:
            return y;
        case Vars::kZ:
            return z;
        case Vars::kW:
            return w;
    }
}

}  // namespace

bool SingleProgram::TryInput(int64_t& x, int64_t& y, int64_t& z,
                             int64_t& w) const {
    // There are 4 variables: x, y, z, w
    // w is provided by the input. x, y & z are defaulted to 0 in the beginning
    // before the start.

    // Process the instructions.
    for (const Instruction& instruction : instructions_) {
        int64_t rhs =
            instruction.IsRhsInt()
                ? instruction.RhsInt()
                : VarsFromCurrentStatus(instruction.RhsVars(), x, y, z, w);
        switch (instruction.op_type()) {
            case Op::kAdd: {
                switch (instruction.lhs()) {
                    case Vars::kX:
                        x = x + rhs;
                        break;
                    case Vars::kY:
                        y = y + rhs;
                        break;
                    case Vars::kZ:
                        z = z + rhs;
                        break;
                    case Vars::kW:
                        w = w + rhs;
                        break;
                }
                break;
            }
            case Op::kMul: {
                switch (instruction.lhs()) {
                    case Vars::kX:
                        x = x * rhs;
                        break;
                    case Vars::kY:
                        y = y * rhs;
                        break;
                    case Vars::kZ:
                        z = z * rhs;
                        break;
                    case Vars::kW:
                        w = w * rhs;
                        break;
                }
                break;
            }
            case Op::kDiv: {
                if (rhs == 0) {
                    return false;
                }
                switch (instruction.lhs()) {
                    case Vars::kX:
                        x = x / rhs;
                        break;
                    case Vars::kY:
                        y = y / rhs;
                        break;
                    case Vars::kZ:
                        z = z / rhs;
                        break;
                    case Vars::kW:
                        w = w / rhs;
                        break;
                }
                break;
            }
            case Op::kMod: {
                if (rhs <= 0) {
                    return false;
                }
                switch (instruction.lhs()) {
                    case Vars::kX:
                        if (x < 0) {
                            return false;
                        }
                        x = x % rhs;
                        break;
                    case Vars::kY:
                        if (y < 0) {
                            return false;
                        }
                        y = y % rhs;
                        break;
                    case Vars::kZ:
                        if (z < 0) {
                            return false;
                        }
                        z = z % rhs;
                        break;
                    case Vars::kW:
                        if (w < 0) {
                            return false;
                        }
                        w = w % rhs;
                        break;
                }
                break;
            }
            case Op::kEq: {
                // eql a b - If the value of a and b are equal, then store the
                // value 1 in variable a. Otherwise, store the value 0 in
                // variable a.
                switch (instruction.lhs()) {
                    case Vars::kX:
                        x = (x == rhs) ? 1 : 0;
                        break;
                    case Vars::kY:
                        y = (y == rhs) ? 1 : 0;
                        break;
                    case Vars::kZ:
                        z = (z == rhs) ? 1 : 0;
                        break;
                    case Vars::kW:
                        w = (w == rhs) ? 1 : 0;
                        break;
                }
                break;
            }
        }
    }
    return true;
}

namespace {

absl::InlinedVector<int, 6> To6Array(int64_t in) {
    if (in == 0) {
        return {};
    }
    absl::InlinedVector<int, 6> ret;
    while (in > 0) {
        const int digit = in % 10;
        if (digit == 0) {
            return {};
        }
        ret.push_back(digit);
        in /= 10;
    }
    assert(ret.size() == 6);
    return ret;
}

absl::InlinedVector<int, 8> To8Array(int in) {
    absl::InlinedVector<int, 8> ret;
    while (in > 0) {
        const int digit = in % 10;

        if (digit == 0) {
            return {};
        }
        ret.push_back(digit);
        in /= 10;
    }
    return ret;
}

}  // namespace

int64_t Parser::ParallelFinder() {
    int64_t winner = std::numeric_limits<int64_t>::max();
    absl::Mutex winner_mu;
    CHECK(thread_pool_->size() == 11);
    absl::BlockingCounter counter(11);
    for (int i = 0; i < 11; ++i) {
        thread_pool_->Schedule([this, i, &counter, &winner, &winner_mu]() {
            int start = 453'111 + i;
            for (int loop = start; loop <= 459'999; loop = loop + 11) {
                absl::InlinedVector<int, 6> start_seq = To6Array(loop);
                if (start_seq.empty()) continue;
                int64_t candidate = LargestModelNumber(start_seq);
                if (candidate > 0) {
                    {
                        absl::MutexLock lck(&winner_mu);
                        if (candidate < winner) {
                            winner = candidate;
                        }
                    }
                }
            }
            counter.DecrementCount();
        });
    }
    counter.Wait();
    return winner;
}

namespace {

std::string DPrintVector(const absl::InlinedVector<int, 6>& s) {
    std::string ret;
    for (int i = 5; i >= 0; --i) {
        absl::StrAppend(&ret, s[i]);
    }
    return ret;
}

// Jump a million elements.
inline int64_t Jump(const int64_t start) {
    if (start % 10 != 0 || start % 100 != 0) {
        return start;
    }

    if (start % 10'000'000 == 0) {
        return start + 1'111'110;
    } else if (start % 1'000'000 == 0) {
        return start + 111'110;
    } else if (start % 100'000 == 0) {
        return start + 11'110;
    } else if (start % 10'000 == 0) {
        return start + 1110;
    } else if (start % 1000 == 0) {
        return start + 110;
    }
    return start;
}

}  // namespace

int64_t Parser::LargestModelNumber(const absl::InlinedVector<int, 6>& starter) {
    // Precompute the x, y and z that comes out of the first 3 digits.
    int64_t xx = 0;
    int64_t yy = 0;
    int64_t zz = 0;

    for (int i = 0; i < 6; ++i) {
        const SingleProgram& sp = programs_[i];
        int64_t w = starter[5 - i];
        if (!sp.TryInput(xx, yy, zz, w)) {
            return -1;
        }
    }

    constexpr int64_t start_3 = 99'999'999;
    constexpr int64_t end_3 = 11'111'111;
    for (int64_t loop = end_3; loop <= start_3; ++loop) {
        absl::InlinedVector<int, 8> in_arr = To8Array(loop);
        if (in_arr.empty()) {
            loop = Jump(loop);
            continue;
        }
        int64_t x = xx;
        int64_t y = yy;
        int64_t z = zz;
        for (int i = 6; i < programs_.size(); ++i) {
            const SingleProgram& sp = programs_[i];
            int64_t w_pp = in_arr[13 - i];
            if (!sp.TryInput(x, y, z, w_pp)) {
                break;
            }
        }
        if (z == 0) {
            std::cout << "Found z = 0 at " << DPrintVector(starter) << loop << std::endl;
            return loop;
        }
    }
    return -1;
}

Parser::Parser(absl::Span<const std::string> strings) {
    std::vector<std::string> current;
    for (const std::string& line : strings) {
        if (absl::StartsWith(line, "inp")) {
            // Start of a new program.
            if (!current.empty()) {
                programs_.emplace_back(current);
            }
            current.clear();
            continue;
        }
        current.push_back(line);
    }
    programs_.emplace_back(current);
    thread_pool_ = std::make_unique<common::ThreadPool>(11);
}

std::string Parser::DebugPrint() const {
    std::string ret;
    for (const SingleProgram& sp : programs_) {
        absl::StrAppend(&ret, sp.DebugPrint());
    }
    return ret;
}

}  // namespace aoc2022
