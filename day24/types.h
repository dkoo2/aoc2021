#pragma once

#include "absl/strings/string_view.h"

namespace aoc2022 {

inline constexpr absl::string_view kMulString = "mul";
inline constexpr absl::string_view kAddString = "add";
inline constexpr absl::string_view kModString = "mod";
inline constexpr absl::string_view kEqString = "eql";
inline constexpr absl::string_view kDivString = "div";

inline constexpr absl::string_view kXString = "x";
inline constexpr absl::string_view kYString = "y";
inline constexpr absl::string_view kZString = "z";
inline constexpr absl::string_view kWString = "w";

enum class Op {
    kAdd = 0,
    kMul = 1,
    kDiv = 2,
    kMod = 3,
    kEq = 4,
};

enum class Vars {
    kX = 0,
    kY = 1,
    kZ = 2,
    kW = 3,
};

inline absl::string_view OpToString(Op op) {
    switch (op) {
        case Op::kAdd:
            return kAddString;
        case Op::kMul:
            return kMulString;
        case Op::kDiv:
            return kDivString;
        case Op::kMod:
            return kModString;
        case Op::kEq:
            return kEqString;
    }
}

inline absl::string_view VarsToString(Vars v) {
    switch (v) {
        case Vars::kX:
            return kXString;
        case Vars::kY:
            return kYString;
        case Vars::kZ:
            return kZString;
        case Vars::kW:
            return kWString;
    }
}

}  // namespace aoc2022