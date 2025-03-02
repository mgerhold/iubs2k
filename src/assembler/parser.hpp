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
        Lexer m_lexer;
        tl::optional<Token> m_current;

    public:
        [[nodiscard]] explicit Parser(std::string_view filename, std::string_view source);

        [[nodiscard]] tl::expected<Instruction, Error> next_instruction();

    private:
        [[nodiscard]] tl::expected<Token, Error> current();

        [[nodiscard]] tl::expected<void, Error> advance();

        [[nodiscard]] tl::expected<std::vector<std::unique_ptr<Operand>>, Error> operands();

        [[nodiscard]] tl::expected<std::unique_ptr<Operand>, Error> operand();
    };
}  // namespace assembler
