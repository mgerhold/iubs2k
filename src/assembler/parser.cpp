#include "parser.hpp"

namespace assembler {
    [[nodiscard]] Parser::Parser(std::string_view const filename, std::string_view const source)
        : m_lexer{ filename, source } {}

    [[nodiscard]] tl::expected<Instruction, Error> Parser::next_instruction() {
        while (true) {
            if (not current().has_value()) {
                return tl::unexpected{ current().error() };
            }
            auto const token = current().value();
            if (token.type() != TokenType::Newline) {
                break;
            }
            if (auto const result = advance(); not result.has_value()) {
                return tl::unexpected{ result.error() };
            }
        }
        auto const token = current().value();
        switch (token.type()) {
            using enum TokenType;
            case EndOfInput:
                return tl::unexpected{ InputExhausted{} };
            case Identifier: {
                auto const mnemonic = token;
                if (auto const result = advance(); not result.has_value()) {
                    return tl::unexpected{ result.error() };
                }
                auto operands = this->operands();
                if (not operands.has_value()) {
                    return tl::unexpected{ operands.error() };
                }
                return Instruction{ mnemonic, std::move(operands).value() };
            }
            default:
                return tl::unexpected{
                    UnexpectedToken{ token, "Expected identifier." }
                };
        }
    }

    [[nodiscard]] tl::expected<Token, Error> Parser::current() {
        if (m_current.has_value()) {
            return m_current.value();
        }
        auto const next = m_lexer.next_token();
        if (not next.has_value()) {
            return tl::unexpected{ next.error() };
        }
        m_current = next.value();
        return m_current.value();
    }

    [[nodiscard]] tl::expected<void, Error> Parser::advance() {
        if (auto const maybe_current = current(); not maybe_current.has_value()) {
            return tl::unexpected{ maybe_current.error() };
        }
        if (current().value().type() == TokenType::EndOfInput) {
            return {};
        }
        auto const next = m_lexer.next_token();
        if (not next.has_value()) {
            return tl::unexpected{ next.error() };
        }
        m_current = next.value();
        return {};
    }

    [[nodiscard]] tl::expected<std::vector<std::unique_ptr<Operand>>, Error> Parser::operands() {
        auto operands = std::vector<std::unique_ptr<Operand>>{};
        while (true) {
            auto token = current();
            if (not token.has_value()) {
                return tl::unexpected{ token.error() };
            }

            if (token.value().type() == TokenType::EndOfInput or token.value().type() == TokenType::Newline) {
                return operands;
            }

            auto operand = this->operand();
            if (not operand.has_value()) {
                return tl::unexpected{ operand.error() };
            }

            operands.push_back(std::move(operand).value());

            token = current();
            if (not token.has_value()) {
                return tl::unexpected{ token.error() };
            }

            if (token.value().type() != TokenType::Comma) {
                return operands;
            }

            if (auto const result = advance(); not result.has_value()) {
                return tl::unexpected{ result.error() };
            }
        }
    }

    [[nodiscard]] tl::expected<std::unique_ptr<Operand>, Error> Parser::operand() {
        if (not current().has_value()) {
            return tl::unexpected{ current().error() };
        }
        switch (current().value().type()) {
            case TokenType::Register: {
                auto register_ = std::make_unique<Register>(current().value());
                if (auto const result = advance(); not result.has_value()) {
                    return tl::unexpected{ result.error() };
                }
                return register_;
            }
            case TokenType::Identifier: {
                auto identifier = std::make_unique<Identifier>(current().value());
                if (auto const result = advance(); not result.has_value()) {
                    return tl::unexpected{ result.error() };
                }
                return identifier;
            }
            case TokenType::Integer: {
                auto immediate = std::make_unique<Immediate>(current().value());
                if (auto const result = advance(); not result.has_value()) {
                    return tl::unexpected{ result.error() };
                }
                return immediate;
            }
            case TokenType::Asterisk: {
                auto const asterisk = current().value();
                if (auto const result = advance(); not result.has_value()) {
                    return tl::unexpected{ result.error() };
                }
                auto pointee = this->operand();
                if (not pointee.has_value()) {
                    return tl::unexpected{ pointee.error() };
                }
                return std::make_unique<Pointer>(asterisk, std::move(pointee).value());
            }
            case TokenType::Comma:
                return tl::unexpected{
                    UnexpectedToken{ current().value(), "Expected operand." }
                };
            case TokenType::EndOfInput:
            case TokenType::Newline:
                throw std::logic_error{ "Token type should have been checked before." };
        }
        throw std::logic_error{ "Unreachable." };
    }

}  // namespace assembler
