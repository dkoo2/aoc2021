#include "instruction.h"

#include <iostream>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "types.h"

namespace aoc2022 {

namespace {

Op OpFromString(absl::string_view str) {
    if (str == kAddString) {
        return Op::kAdd;
    } else if (str == kMulString) {
        return Op::kMul;
    } else if (str == kDivString) {
        return Op::kDiv;
    } else if (str == kEqString) {
        return Op::kEq;
    } else if (str == kModString) {
        return Op::kMod;
    }
    CHECK(false);
}

Vars VarsFromString(absl::string_view str) {
    if (str == kXString) {
        return Vars::kX;
    } else if (str == kYString) {
        return Vars::kY;
    } else if (str == kZString) {
        return Vars::kZ;
    } else if (str == kWString) {
        return Vars::kW;
    }
    CHECK(false);
}

}  // namespace

std::string Instruction::Print() const {
    return absl::StrCat(
        OpToString(type_), " ", VarsToString(lhs_), " ",
        IsRhsVars() ? VarsToString(RhsVars()) : std::to_string(RhsInt()));
}

Instruction::Instruction(absl::string_view str) {
    const std::vector<std::string> parts = absl::StrSplit(str, " ");
    CHECK_EQ(parts.size(), 3);
    absl::string_view op_str = parts[0];
    absl::string_view lhs_str = parts[1];
    absl::string_view rhs_str = parts[2];

    type_ = OpFromString(op_str);
    lhs_ = VarsFromString(lhs_str);

    int try_parse;
    if (absl::SimpleAtoi(rhs_str, &try_parse)) {
        rhs_ = try_parse;
    } else {
        rhs_ = VarsFromString(rhs_str);
    }
}

}  // namespace aoc2022