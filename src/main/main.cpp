#include <fmt/format.h>
#include <assembler/assembler.hpp>
#include <cassert>
#include <iterator>
#include <common/instruction.hpp>
#include <common/pointer.hpp>
#include <emulator/emulator.hpp>
#include <gui/gui.hpp>
#include <string_view>
#include <vector>

int main() {
    using namespace std::string_view_literals;
    static constexpr auto assembly = R"(
copy 0, A
copy 0x6C6C6548, *A ; "Hell"
copy 4, A
copy 0x77202C6F, *A ; "o, w"
copy 8, A
copy 0x646C726F, *A ; "orld"
copy 12, A
copy 0x00000021, *A ; "!"
halt
)"sv;
    auto assembler = assembler::Assembler{ "test.asm", assembly };
    auto instruction_memory = std::vector<std::byte>{};
    while (true) {
        auto const instruction = assembler.next_instruction();
        if (not instruction.has_value()) {
            if (std::holds_alternative<assembler::InputExhausted>(instruction.error())) {
                break;
            }
            if (auto const source_location = instruction.error().source_location()) {
                auto const& location = source_location.value();
                fmt::println(
                    std::cerr,
                    "{}:{}:{}: {}",
                    location.filename(),
                    location.row(),
                    location.column(),
                    instruction.error()
                );
                auto const num_digits = static_cast<int>(std::to_string(location.row()).length());
                auto const surrounding_line = location.surrounding_line();
                fmt::println(std::cerr, "{} | {}", location.row(), surrounding_line);
                auto const offset = std::distance(surrounding_line.data(), location.lexeme().data());
                fmt::println(std::cerr, "{:{}} | {:>{}}{:~>{}}^", "", num_digits, "", offset, "", location.lexeme().length() - 1);
            } else {
                fmt::println(std::cerr, "Error: {}", instruction.error());
            }
            return EXIT_FAILURE;
        }
        instruction.value().encode(std::back_inserter(instruction_memory));
    }

    fmt::println("Decoded memory:");
    for (auto const& instruction : decode(instruction_memory)) {
        fmt::println("{}", instruction);
    }

    auto gui = Gui{};
    auto emulator = Emulator{ instruction_memory };

    while (gui.is_running()) {
        if (not emulator.is_halted()) {
            emulator.step();
        }
        gui.update(emulator);
    }
}
