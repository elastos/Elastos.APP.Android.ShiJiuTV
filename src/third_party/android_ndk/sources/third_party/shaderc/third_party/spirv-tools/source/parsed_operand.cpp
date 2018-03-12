// Copyright (c) 2016 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This file contains utility functions for spv_parsed_operand_t.

#include "parsed_operand.h"

#include <cassert>
#include "util/hex_float.h"

namespace libspirv {

void EmitNumericLiteral(std::ostream* out, const spv_parsed_instruction_t& inst,
                        const spv_parsed_operand_t& operand) {
  assert(operand.type == SPV_OPERAND_TYPE_LITERAL_INTEGER ||
         operand.type == SPV_OPERAND_TYPE_TYPED_LITERAL_NUMBER);
  assert(1 <= operand.num_words);
  assert(operand.num_words <= 2);

  const uint32_t word = inst.words[operand.offset];
  if (operand.num_words == 1) {
    switch (operand.number_kind) {
      case SPV_NUMBER_SIGNED_INT:
        *out << int32_t(word);
        break;
      case SPV_NUMBER_UNSIGNED_INT:
        *out << word;
        break;
      case SPV_NUMBER_FLOATING:
        if (operand.number_bit_width == 16) {
          *out << spvutils::FloatProxy<spvutils::Float16>(
              uint16_t(word & 0xFFFF));
        } else {
          // Assume 32-bit floats.
          *out << spvutils::FloatProxy<float>(word);
        }
        break;
      default:
        assert(false && "Unreachable");
    }
  } else if (operand.num_words == 2) {
    // Multi-word numbers are presented with lower order words first.
    uint64_t bits =
        uint64_t(word) | (uint64_t(inst.words[operand.offset + 1]) << 32);
    switch (operand.number_kind) {
      case SPV_NUMBER_SIGNED_INT:
        *out << int64_t(bits);
        break;
      case SPV_NUMBER_UNSIGNED_INT:
        *out << bits;
        break;
      case SPV_NUMBER_FLOATING:
        // Assume only 64-bit floats.
        *out << spvutils::FloatProxy<double>(bits);
        break;
      default:
        assert(false && "Unreachable");
    }
  } else {
    // TODO(dneto): Support more than 64-bits at a time.
    assert(false && "Unhandled");
  }
}
}  // namespace libspirv
