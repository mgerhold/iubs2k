#pragma once

#include <common/instruction.hpp>
#include <string_view>
#include "error.hpp"

namespace assembler {
    [[nodiscard]] tl::expected<std::vector<::Instruction>, Error> assemble(
        std::string_view filename,
        std::string_view source
    );
}
