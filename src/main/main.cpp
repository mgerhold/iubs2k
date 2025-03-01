#include <fmt/format.h>
#include <cassert>
#include <common/instruction.hpp>
#include <common/pointer.hpp>
#include <emulator/emulator.hpp>
#include <gui/gui.hpp>
#include <vector>

int main() {
    auto const instructions = std::vector<Instruction>{
        MoveImmediateIntoRegister{ 0, Register::A }, // copy 0x0, A
        MoveImmediateIntoMemory{ 0x6C6C6548, *Register::A }, // copy 0x6C6C6548, *A ; "Hell"
        MoveImmediateIntoRegister{ 4, Register::A }, // copy 0x4, A
        MoveImmediateIntoMemory{ 0x77202C6F, *Register::A }, // copy 0x6C6C6548, *A ; "o, w"
        MoveImmediateIntoRegister{ 8, Register::A }, // copy 0x8, A
        MoveImmediateIntoMemory{ 0x646C726F, *Register::A }, // copy 0x646C726F, *A ; "orld"
        MoveImmediateIntoRegister{ 12, Register::A }, // copy 0xB, A
        MoveImmediateIntoMemory{ 0x00000021, *Register::A }, // copy 0x00000021, *A ; "!"
        HaltAndCatchFire{}, // halt
    };

    auto instruction_memory = std::vector<std::byte>{};
    for (auto const& instruction : instructions) {
        instruction.encode(std::back_inserter(instruction_memory));
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
