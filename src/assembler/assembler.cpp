#include <assembler/assembler.hpp>
#include "parser.hpp"

namespace assembler {
    [[nodiscard]] tl::expected<std::vector<::Instruction>, Error> assemble(
        std::string_view const filename,
        std::string_view const source
    ) {
        auto lexer = Lexer{ filename, source };
        if (auto const result = lexer.tokenize(); not result.has_value()) {
            return tl::unexpected{ result.error() };
        }
        auto parser = Parser{ std::move(lexer).take() };
        if (auto const result = parser.parse(); not result.has_value()) {
            return tl::unexpected{ result.error() };
        }
        auto const instructions = std::move(parser).take();

        auto result = std::vector<::Instruction>{};
        for (auto const& instruction : instructions) {
            auto lowered = instruction.lower();
            if (not lowered.has_value()) {
                return tl::unexpected{ lowered.error() };
            }
            result.push_back(std::move(lowered).value());
        }
        return result;
    }
}  // namespace assembler
