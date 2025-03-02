#include <assembler/instruction.hpp>

namespace assembler {
    [[nodiscard]] tl::expected<::Instruction, Error> Instruction::lower() const {
        if (m_mnemonic.type() != TokenType::Identifier) {
            throw std::logic_error{ "Mnemonic must be identifier." };
        }

        if (m_mnemonic.lexeme() == "halt") {
            if (not m_operands.empty()) {
                return tl::unexpected{
                    ArityMismatch{ m_mnemonic, 0, m_operands.size() }
                };
            }

            return HaltAndCatchFire{};
        }

        if (m_mnemonic.lexeme() == "copy") {
            if (m_operands.size() != 2) {
                return tl::unexpected{
                    ArityMismatch{ m_mnemonic, 2, m_operands.size() }
                };
            }

            auto const lhs_immediate = m_operands[0]->as_immediate();
            auto const rhs_register = m_operands[1]->as_register();
            if (lhs_immediate.has_value() and rhs_register.has_value()) {
                auto const immediate_value = lhs_immediate.value().value();
                if (not immediate_value.has_value()) {
                    return tl::unexpected{ immediate_value.error() };
                }
                return MoveImmediateIntoRegister{ immediate_value.value(), rhs_register.value().value() };
            }

            auto const rhs_pointer = m_operands[1]->as_pointer();
            if (lhs_immediate.has_value() and rhs_pointer.has_value()) {
                auto const immediate_value = lhs_immediate.value().value();
                if (not immediate_value.has_value()) {
                    return tl::unexpected{ immediate_value.error() };
                }
                auto const& pointee = rhs_pointer.value().pointee().as_register();
                if (not pointee.has_value()) {
                    return tl::unexpected{ InvalidOperands{ m_mnemonic } };
                }
                return MoveImmediateIntoMemory{ immediate_value.value(), *pointee.value().value() };
            }

            return tl::unexpected{ InvalidOperands{ m_mnemonic } };
        }

        return tl::unexpected{ UnknownMnemonic{ m_mnemonic } };
    }
}  // namespace assembler
