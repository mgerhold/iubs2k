#pragma once

#include <assembler/error.hpp>
#include <lib2k/types.hpp>
#include <string_view>
#include <tl/expected.hpp>
#include <assembler/token.hpp>

namespace assembler {
    class Lexer final {
    private:
        std::string_view m_filename;
        std::string_view m_source;
        usize m_index = 0;
        usize m_row = 1;
        usize m_column = 1;
        bool m_input_exhausted = false;

    public:
        [[nodiscard]] explicit Lexer(std::string_view filename, std::string_view source);

        [[nodiscard]] tl::expected<Token, Error> next_token();

    private:
        [[nodiscard]] bool is_at_end() const;

        [[nodiscard]] char current() const;

        [[nodiscard]] char peek() const;

        void advance();

        [[nodiscard]] SourceLocation current_source_location(usize length = 1) const;

        [[nodiscard]] SourceLocation source_location_from(usize start_offset, usize start_row, usize start_column) const;

        [[nodiscard]] tl::expected<Token, Error> integer();

        [[nodiscard]] tl::expected<Token, Error> identifier();

        [[nodiscard]] static bool is_valid_identifier_start(char c);

        [[nodiscard]] static bool is_valid_identifier_continuation(char c);
    };
}  // namespace assembler
