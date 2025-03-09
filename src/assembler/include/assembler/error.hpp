#pragma once

#include <tl/expected.hpp>
#include <tl/optional.hpp>
#include <variant>
#include "source_location.hpp"
#include "token.hpp"

namespace assembler {
    struct InputExhausted {
        [[nodiscard]] friend char const* format_as(InputExhausted const&) {
            return "Input exhausted.";
        }
    };

    struct InvalidChar final {
        SourceLocation source_location;

        [[nodiscard]] explicit InvalidChar(SourceLocation const& source_location)
            : source_location{ source_location } {}

        [[nodiscard]] friend std::string format_as(InvalidChar const& error) {
            return fmt::format("Invalid character: '{}'", error.source_location.lexeme());
        }
    };

    struct InvalidInteger final {
        SourceLocation source_location;

        [[nodiscard]] explicit InvalidInteger(SourceLocation const& source_location)
            : source_location{ source_location } {}

        [[nodiscard]] friend std::string format_as(InvalidInteger const& error) {
            return fmt::format("Invalid integer: '{}'", error.source_location.lexeme());
        }
    };

    struct UnexpectedToken final {
        Token token;
        std::string message;

        [[nodiscard]] explicit UnexpectedToken(Token const& token, std::string message)
            : token{ token }, message{ std::move(message) } {}

        [[nodiscard]] friend std::string format_as(UnexpectedToken const& error) {
            return fmt::format("Unexpected token: '{}'. {}", error.token.lexeme(), error.message);
        }
    };

    struct ArityMismatch final {
        Token mnemonic;
        usize expected;
        usize actual;

        [[nodiscard]] explicit ArityMismatch(Token const& mnemonic, usize const expected, usize const actual)
            : mnemonic{ mnemonic }, expected{ expected }, actual{ actual } {}

        [[nodiscard]] friend std::string format_as(ArityMismatch const& error) {
            return fmt::format(
                "Arity mismatch for mnemonic '{}'. Expected {}, but got {}.",
                error.mnemonic.lexeme(),
                error.expected,
                error.actual
            );
        }
    };

    struct InvalidOperands final {
        Token mnemonic;

        [[nodiscard]] explicit InvalidOperands(Token const& mnemonic)
            : mnemonic{ mnemonic } {}

        [[nodiscard]] friend std::string format_as(InvalidOperands const& error) {
            return fmt::format("Invalid operands for mnemonic '{}'.", error.mnemonic.lexeme());
        }
    };

    struct UnknownMnemonic final {
        Token mnemonic;

        [[nodiscard]] explicit UnknownMnemonic(Token const& mnemonic)
            : mnemonic{ mnemonic } {}

        [[nodiscard]] friend std::string format_as(UnknownMnemonic const& error) {
            return fmt::format("Unknown mnemonic: '{}'", error.mnemonic.lexeme());
        }
    };

    // clang-format off
    using ErrorBase = std::variant<
        InputExhausted,
        InvalidChar,
        InvalidInteger,
        UnexpectedToken,
        ArityMismatch,
        UnknownMnemonic,
        InvalidOperands
    >;
    // clang-format on

    class Error final : public ErrorBase {
        using ErrorBase::ErrorBase;

    public:
        [[nodiscard]] tl::optional<SourceLocation> source_location() const {
            // clang-format off
            return std::visit(
                [](auto const& error) -> tl::optional<SourceLocation> {
                    if constexpr (requires { { error.source_location } -> std::convertible_to<SourceLocation>; }) {
                        return static_cast<SourceLocation>(error.source_location);
                    } else if constexpr (requires { { error.source_location() } -> std::convertible_to<SourceLocation>; }) {
                        return static_cast<SourceLocation>(error.source_location());
                    } else if constexpr (requires { { error.mnemonic } -> std::convertible_to<Token>; }) {
                        return static_cast<Token>(error.mnemonic).source_location();
                    } else if constexpr (requires{ {error.token } -> std::convertible_to<Token>; } ) {
                         return static_cast<Token>(error.token).source_location();
                    } else {
                        return tl::nullopt;
                    }
                },
                *this
            );
            // clang-format on
        }

        [[nodiscard]] friend std::string format_as(Error const& error) {
            return std::visit(
                [](auto const& e) {
                    static_assert(requires { format_as(e); }, "Variant alternative must implement format_as().");
                    return fmt::format("{}", e);
                },
                error
            );
        }
    };
}  // namespace assembler
