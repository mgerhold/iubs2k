#include <assembler/assembler.hpp>
#include "parser.hpp"

namespace assembler {
    [[nodiscard]] Assembler::Assembler(std::string_view const filename, std::string_view const source)
        : m_parser{ std::make_unique<Parser>(filename, source) } {}

    Assembler::~Assembler() = default;

    [[nodiscard]] tl::expected<::Instruction, Error> Assembler::next_instruction() {
        auto const instruction = m_parser->next_instruction();
        if (not instruction.has_value()) {
            return tl::unexpected{ instruction.error() };
        }
        return instruction.value().lower();
    }
}  // namespace assembler
