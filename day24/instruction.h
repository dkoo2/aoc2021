#pragma once

#include <optional>
#include <string>
#include <variant>

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "types.h"

namespace aoc2022 {

class Instruction {
   public:
    explicit Instruction(absl::string_view str);

    Op op_type() const { return type_; }
    Vars lhs() const { return lhs_; }
    std::string Print() const;

    bool IsRhsVars() const { return std::holds_alternative<Vars>(rhs_); }
    bool IsRhsInt() const { return std::holds_alternative<int>(rhs_); }

    Vars RhsVars() const { return std::get<Vars>(rhs_); }
    int RhsInt() const { return std::get<int>(rhs_); }

   private:
    Op type_;
    Vars lhs_;
    std::variant<Vars, int> rhs_;
};

}  // namespace aoc2022
