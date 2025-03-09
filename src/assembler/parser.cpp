#include "parser.hpp"

namespace assembler {
    [[nodiscard]] Parser::Parser(std::vector<Token> tokens)
        : m_tokens{ std::move(tokens) } {}

    [[nodiscard]] tl::expected<void, Error> Parser::parse() {
        *this = Parser{ std::move(m_tokens) };
        while (true) {
            while (current().type() == TokenType::Newline) {
                advance();
            }
            if (is_at_end()) {
                break;
            }
            auto instruction = this->instruction();
            if (not instruction.has_value()) {
                return tl::unexpected{ instruction.error() };
            }
            m_instructions.push_back(std::move(instruction).value());
        }
        return {};
    }

    [[nodiscard]] std::vector<Instruction> Parser::take() && {
        return std::move(m_instructions);
    }

    [[nodiscard]] tl::expected<Instruction, Error> Parser::instruction() {
        while (true) {
            if (current().type() != TokenType::Newline) {
                break;
            }
            advance();
        }
        switch (current().type()) {
            using enum TokenType;
            case EndOfInput:
                // TODO: Exchange with dedicated "unexpected end of input" error
                return tl::unexpected{ InputExhausted{} };
            case Identifier: {
                auto const mnemonic = current();
                advance();
                auto operands = this->operands();
                if (not operands.has_value()) {
                    return tl::unexpected{ operands.error() };
                }
                return Instruction{ mnemonic, std::move(operands).value() };
            }
            default:
                return tl::unexpected{
                    UnexpectedToken{ current(), "Expected identifier." }
                };
        }
    }

    [[nodiscard]] bool Parser::is_at_end() const {
        return m_index >= m_tokens.size() or m_tokens.at(m_index).type() == TokenType::EndOfInput;
    }

    [[nodiscard]] Token const& Parser::current() const {
        if (is_at_end()) {
            return m_tokens.at(m_tokens.size() - 1);
        }
        return m_tokens.at(m_index);
    }

    void Parser::advance() {
        if (is_at_end()) {
            return;
        }
        ++m_index;
    }

    [[nodiscard]] tl::expected<std::vector<std::unique_ptr<Operand>>, Error> Parser::operands() {
        auto operands = std::vector<std::unique_ptr<Operand>>{};
        while (true) {
            if (current().type() == TokenType::EndOfInput or current().type() == TokenType::Newline) {
                return operands;
            }

            auto operand = this->operand();
            if (not operand.has_value()) {
                return tl::unexpected{ operand.error() };
            }

            operands.push_back(std::move(operand).value());

            if (current().type() != TokenType::Comma) {
                return operands;
            }

            advance();
        }
    }

    [[nodiscard]] tl::expected<std::unique_ptr<Operand>, Error> Parser::operand() {
        switch (current().type()) {
            case TokenType::Register: {
                auto register_ = std::make_unique<Register>(current());
                advance();
                return register_;
            }
            case TokenType::Identifier: {
                auto identifier = std::make_unique<Identifier>(current());
                advance();
                return identifier;
            }
            case TokenType::Integer: {
                auto immediate = std::make_unique<Immediate>(current());
                advance();
                return immediate;
            }
            case TokenType::Asterisk: {
                auto const asterisk = current();
                advance();
                auto pointee = this->operand();
                if (not pointee.has_value()) {
                    return tl::unexpected{ pointee.error() };
                }
                return std::make_unique<Pointer>(asterisk, std::move(pointee).value());
            }
            case TokenType::Comma:
                return tl::unexpected{
                    UnexpectedToken{ current(), "Expected operand." }
                };
            case TokenType::EndOfInput:
            case TokenType::Newline:
                throw std::logic_error{ "Token type should have been checked before." };
        }
        throw std::logic_error{ "Unreachable." };
    }

}  // namespace assembler
