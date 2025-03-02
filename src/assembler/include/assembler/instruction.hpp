#pragma once

#include <assembler/error.hpp>
#include <common/instruction.hpp>
#include <tl/expected.hpp>
#include <vector>
#include "operand.hpp"
#include "token.hpp"

namespace assembler {
    class Instruction final {
    private:
        Token m_mnemonic;
        std::vector<std::unique_ptr<Operand>> m_operands;

    public:
        [[nodiscard]] Instruction(Token const& mnemonic, std::vector<std::unique_ptr<Operand>> operands)
            : m_mnemonic{ mnemonic }, m_operands{ std::move(operands) } {}

        [[nodiscard]] tl::expected<::Instruction, Error> lower() const;
    };
}  // namespace assembler
