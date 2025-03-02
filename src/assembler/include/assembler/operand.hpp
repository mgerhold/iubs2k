#pragma once

#include <common/register.hpp>
#include <lib2k/string_utils.hpp>
#include <magic_enum.hpp>
#include <memory>
#include <tl/optional.hpp>
#include <variant>
#include "token.hpp"

namespace assembler {

    class Identifier;
    class Immediate;
    class Pointer;
    class Register;

    class Operand {
    protected:
        [[nodiscard]] Operand() = default;
        Operand(Operand const& other) = default;
        Operand(Operand&& other) noexcept = default;

    public:
        Operand& operator=(Operand const& other) = delete;
        Operand& operator=(Operand&& other) noexcept = delete;
        virtual ~Operand() = default;

        [[nodiscard]] virtual tl::optional<Register const&> as_register() const {
            return tl::nullopt;
        }

        [[nodiscard]] virtual tl::optional<Identifier const&> as_identifier() const {
            return tl::nullopt;
        }

        [[nodiscard]] virtual tl::optional<Immediate const&> as_immediate() const {
            return tl::nullopt;
        }

        [[nodiscard]] virtual tl::optional<Pointer const&> as_pointer() const {
            return tl::nullopt;
        }
    };

    class Register final : public Operand {
    private:
        Token m_token;

    public:
        [[nodiscard]] explicit Register(Token const& token)
            : m_token{ token } {}

        [[nodiscard]] ::Register value() const {
            return magic_enum::enum_cast<::Register>(m_token.lexeme()).value();
        }

        [[nodiscard]] tl::optional<Register const&> as_register() const override {
            return *this;
        }
    };

    class Identifier final : public Operand {
    private:
        Token m_token;

    public:
        [[nodiscard]] explicit Identifier(Token const& token)
            : m_token{ token } {}

        [[nodiscard]] tl::optional<Identifier const&> as_identifier() const override {
            return *this;
        }
    };

    class Immediate final : public Operand {
    private:
        Token m_token;

    public:
        [[nodiscard]] explicit Immediate(Token const& token)
            : m_token{ token } {}

        [[nodiscard]] tl::optional<Immediate const&> as_immediate() const override {
            return *this;
        }

        [[nodiscard]] tl::expected<Word, Error> value() const {
            auto const is_hex = m_token.lexeme().starts_with("0x");
            auto const view = is_hex ? m_token.lexeme().substr(2) : m_token.lexeme();
            auto const result = c2k::parse<Word>(view, is_hex ? 16 : 10);
            if (not result.has_value()) {
                return tl::unexpected{ InvalidInteger{ m_token.source_location() } };
            }
            return result.value();
        }
    };

    class Pointer final : public Operand {
    private:
        Token m_asterisk;
        std::unique_ptr<Operand> m_pointee;

    public:
        [[nodiscard]] Pointer(Token const& asterisk, std::unique_ptr<Operand> pointee)
            : m_asterisk{ asterisk }, m_pointee{ std::move(pointee) } {}

        [[nodiscard]] tl::optional<Pointer const&> as_pointer() const override {
            return *this;
        }

        [[nodiscard]] Operand const& pointee() const {
            return *m_pointee;
        }
    };
}  // namespace assembler
