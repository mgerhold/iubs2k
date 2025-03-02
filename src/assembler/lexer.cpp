#include "lexer.hpp"
#include <cctype>
#include <common/register.hpp>
#include <ctre.hpp>
#include <magic_enum.hpp>

namespace assembler {
    [[nodiscard]] Lexer::Lexer(std::string_view const filename, std::string_view const source)
        : m_filename{ filename }, m_source{ source } {}

    [[nodiscard]] tl::expected<Token, Error> Lexer::next_token() {
        if (m_input_exhausted) {
            return tl::unexpected{ InputExhausted{} };
        }

        if (is_at_end()) {
            m_input_exhausted = true;
            return Token{
                TokenType::EndOfInput,
                current_source_location(0),
            };
        }

        while (current() != '\n' and std::isspace(static_cast<unsigned char>(current()))) {
            advance();
        }

        switch (current()) {
            case '\n': {
                auto const result = Token{
                    TokenType::Newline,
                    current_source_location(),
                };
                advance();
                return result;
            }
            case ';': {
                while (current() != '\n' and not is_at_end()) {
                    advance();
                }
                return next_token();
            }
            case ',': {
                auto const result = Token{
                    TokenType::Comma,
                    current_source_location(),
                };
                advance();
                return result;
            }
            case '*': {
                auto const result = Token{
                    TokenType::Asterisk,
                    current_source_location(),
                };
                advance();
                return result;
            }
            default:
                if (std::isdigit(static_cast<unsigned char>(current()))) {
                    return integer();
                }
                if (is_valid_identifier_start(current())) {
                    return identifier();
                }
                return tl::unexpected{ InvalidChar{ current_source_location() } };
        }
    }

    [[nodiscard]] tl::expected<Token, Error> Lexer::integer() {
        if (auto const match = ctre::search<"^(0x[0-9A-Fa-f]+|[0-9]+)">(m_source.substr(m_index))) {
            auto const start_offset = m_index;
            auto const start_row = m_row;
            auto const start_column = m_column;
            auto const lexeme = match.get<1>().to_view();
            for (auto const _ : lexeme) {
                advance();
            }
            return Token{
                TokenType::Integer,
                source_location_from(start_offset, start_row, start_column),
            };
        }
        return tl::unexpected{ InvalidInteger{ current_source_location() } };
    }

    [[nodiscard]] tl::expected<Token, Error> Lexer::identifier() {
        assert(is_valid_identifier_start(current()));
        auto const start_offset = m_index;
        auto const start_row = m_row;
        auto const start_column = m_column;
        advance();
        while (is_valid_identifier_continuation(current())) {
            advance();
        }
        auto const lexeme = m_source.substr(start_offset, m_index - start_offset);
        if (magic_enum::enum_cast<Register>(lexeme)) {
            return Token{
                TokenType::Register,
                source_location_from(start_offset, start_row, start_column),
            };
        }
        return Token{
            TokenType::Identifier,
            source_location_from(start_offset, start_row, start_column),
        };
    }

    [[nodiscard]] bool Lexer::is_valid_identifier_start(char const c) {
        return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z');
    }

    [[nodiscard]] bool Lexer::is_valid_identifier_continuation(char const c) {
        return is_valid_identifier_start(c) or (c >= '0' and c <= '9') or (c == '_');
    }

    [[nodiscard]] bool Lexer::is_at_end() const {
        return m_index >= m_source.length();
    }

    [[nodiscard]] char Lexer::current() const {
        return is_at_end() ? '\0' : m_source[m_index];
    }

    [[nodiscard]] char Lexer::peek() const {
        if (m_index + 1 >= m_source.length()) {
            return '\0';
        }
        return m_source[m_index + 1];
    }

    void Lexer::advance() {
        if (is_at_end()) {
            return;
        }
        if (current() == '\n') {
            ++m_row;
            m_column = 0;
        }
        ++m_column;
        ++m_index;
    }

    [[nodiscard]] SourceLocation Lexer::current_source_location(usize const length) const {
        return SourceLocation{ m_filename, m_source, m_index, length, m_row, m_column };
    }

    [[nodiscard]] SourceLocation Lexer::source_location_from(
        usize const start_offset,
        usize const start_row,
        usize const start_column
    ) const {
        return SourceLocation{ m_filename, m_source, start_offset, m_index - start_offset, start_row, start_column };
    }

}  // namespace assembler
