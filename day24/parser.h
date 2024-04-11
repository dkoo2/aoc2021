#pragma once

#include <span>
#include <string>
#include <vector>
#include <mutex>

#include "absl/container/flat_hash_set.h"
#include "absl/container/inlined_vector.h"
#include "absl/types/span.h"
#include "instruction.h"
#include "thread_pool.h"

namespace aoc2022 {

// A program is made up of one or more instructions. The start of the program,
// the "input" is left out.
class SingleProgram {
   public:
    explicit SingleProgram(absl::Span<const std::string> strings);
    std::string DebugPrint() const;

    bool TryInput(int64_t& x, int64_t& y, int64_t& z, int64_t& w) const;

   private:
    std::vector<Instruction> instructions_;
};

// Parser takes in the input file as a list of strings and generates
// a list of programs.
class Parser {
   public:
    explicit Parser(absl::Span<const std::string> strings);

    std::string DebugPrint() const;

    int64_t ParallelFinder();
    int64_t LargestModelNumber(const absl::InlinedVector<int, 6>& starter);

   private:
    std::unique_ptr<common::ThreadPool> thread_pool_ = nullptr;
    std::vector<SingleProgram> programs_;
};

}  // namespace aoc2022