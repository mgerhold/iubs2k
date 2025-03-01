#include <algorithm>
#include <common/common.hpp>
#include <common/instruction.hpp>
#include <emulator/emulator.hpp>
#include <lib2k/overloaded.hpp>
#include <ranges>
#include <span>

[[nodiscard]] Emulator::Emulator(std::span<std::byte const> const memory)
    : m_memory(TextDevice::num_mapped_bytes + memory.size()),
      m_text_device{ std::span{ m_memory }.subspan(0, TextDevice::num_mapped_bytes) } {
    static constexpr auto entry_point_offset = TextDevice::num_mapped_bytes;
    std::ranges::copy(memory, m_memory.begin() + entry_point_offset);
    m_instruction_pointer = entry_point_offset;
}

void Emulator::step() {
    if (is_halted()) {
        throw std::runtime_error{ "Emulator is halted" };
    }

    auto const instruction =
        Instruction::decode(std::span{ &m_memory.at(m_instruction_pointer), m_memory.data() + m_memory.size() });

    std::visit(
        c2k::Overloaded{
            [this](HaltAndCatchFire const&) { m_is_halted = true; },
            [this](MoveImmediateIntoRegister const& inst) { write_register(inst.register_, inst.immediate); },
            [this](MoveImmediateIntoMemory const& inst) { write_into_memory(inst.pointer, inst.immediate); },
        },
        instruction
    );

    m_instruction_pointer += instruction.byte_length();
}
