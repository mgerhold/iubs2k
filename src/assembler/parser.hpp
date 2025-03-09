#pragma once

#include <assembler/error.hpp>
#include <assembler/instruction.hpp>
#include <string_view>
#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include "lexer.hpp"

namespace assembler {
    class Parser final {
    private:
        std::vector<Token> m_tokens;
        std::vector<Instruction> m_instructions;
        usize m_index = 0;

    public:
        [[nodiscard]] explicit Parser(std::vector<Token> tokens);

        [[nodiscard]] tl::expected<void, Error> parse();

        [[nodiscard]] std::vector<Instruction> take() &&;

    private:
        [[nodiscard]] tl::expected<Instruction, Error> instruction();

        [[nodiscard]] bool is_at_end() const;

        [[nodiscard]] Token const& current() const;

        void advance();

        [[nodiscard]] tl::expected<std::vector<std::unique_ptr<Operand>>, Error> operands();

        [[nodiscard]] tl::expected<std::unique_ptr<Operand>, Error> operand();
    };
}  // namespace assembler
