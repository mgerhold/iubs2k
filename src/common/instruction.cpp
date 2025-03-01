#include <common/instruction.hpp>

#include <algorithm>
#include <concepts>
#include <magic_enum.hpp>
#include <utility>

void write_into(Opcode const opcode, std::span<std::byte> const buffer) {
    buffer[0] = static_cast<std::byte>(std::to_underlying(opcode));
}

void write_into(Register const register_, std::span<std::byte> const buffer) {
    buffer[0] = static_cast<std::byte>(std::to_underlying(register_));
}

void write_into(Pointer const pointer, std::span<std::byte> const buffer) {
    write_into(pointer.register_(), buffer);
}

void write_into(std::integral auto value, std::span<std::byte> const buffer) {
    if (buffer.size() < sizeof(value)) {
        throw std::runtime_error{ "Insufficient buffer size." };
    }

    value = to_little_endian(value);
    auto const begin = reinterpret_cast<std::byte const*>(&value);
    auto const end = begin + sizeof(value);
    std::copy(begin, end, buffer.begin());
}

template<typename T>
[[nodiscard]] T read_from(std::span<std::byte const> const buffer)
    requires(std::integral<T> or std::same_as<T, Register>)
{
    if (buffer.size() < sizeof(T)) {
        throw std::runtime_error{ "Insufficient buffer size." };
    }
    auto value = T{};
    auto const begin = reinterpret_cast<std::byte*>(&value);
    std::copy_n(buffer.begin(), sizeof(T), begin);
    return value;
}

template<>
[[nodiscard]] Register read_from<Register>(std::span<std::byte const> const buffer) {
    return magic_enum::enum_cast<Register>(std::to_integer<std::underlying_type_t<Register>>(buffer[0])).value();
}

/*[[nodiscard]] std::unique_ptr<Instruction> Instruction::decode(std::span<std::byte const> memory) {
    if (memory.empty()) {
        throw std::runtime_error{ "Unable to decode instruction from empty memory." };
    }
    auto const opcode = magic_enum::enum_cast<Opcode>(static_cast<std::underlying_type_t<Opcode>>(memory[0]));
    if (not opcode.has_value()) {
        throw std::runtime_error{ std::format("Invalid opcode: 0x{:x}", static_cast<int>(memory[0])) };
    }

    switch (opcode.value()) {
        case Opcode::HaltAndCatchFire:
            return std::make_unique<HaltAndCatchFire>();
    }

    throw std::runtime_error{ "Unreachable." };
}*/

void HaltAndCatchFire::encode_into(std::span<std::byte> const buffer) const {
    write_into(opcode, buffer);
}

[[nodiscard]] Instruction HaltAndCatchFire::decode(std::span<std::byte const> const buffer) {
    assert(std::to_integer<std::underlying_type_t<Opcode>>(buffer[0]) == std::to_underlying(opcode));
    return HaltAndCatchFire{};
}

[[nodiscard]] Instruction MoveImmediateIntoRegister::decode(std::span<std::byte const> const buffer) {
    assert(std::to_integer<std::underlying_type_t<Opcode>>(buffer[0]) == std::to_underlying(opcode));
    auto const immediate = read_from<Word>(buffer.subspan(1));
    auto const register_ = read_from<Register>(buffer.subspan(1 + sizeof(immediate)));
    return MoveImmediateIntoRegister{ immediate, register_ };
}

void MoveImmediateIntoMemory::encode_into(std::span<std::byte> const buffer) const {
    write_into(opcode, buffer.subspan(0));
    write_into(immediate, buffer.subspan(1));
    write_into(pointer, buffer.subspan(1 + sizeof(immediate)));
}

[[nodiscard]] Instruction MoveImmediateIntoMemory::decode(std::span<std::byte const> buffer) {
    assert(std::to_integer<std::underlying_type_t<Opcode>>(buffer[0]) == std::to_underlying(opcode));
    auto const immediate = read_from<Word>(buffer.subspan(1));
    auto const register_ = read_from<Register>(buffer.subspan(1 + sizeof(immediate)));
    return MoveImmediateIntoMemory{ immediate, *register_ };
}

void MoveImmediateIntoRegister::encode_into(std::span<std::byte> const buffer) const {
    write_into(opcode, buffer.subspan(0));
    write_into(immediate, buffer.subspan(1));
    write_into(register_, buffer.subspan(1 + sizeof(immediate)));
}

[[nodiscard]] std::vector<Instruction> decode(std::span<std::byte const> const memory) {
    auto instructions = std::vector<Instruction>{};
    auto subspan = memory.subspan(0);
    while (not subspan.empty()) {
        auto instruction = Instruction::decode(subspan);
        subspan = subspan.subspan(instruction.byte_length());
        instructions.push_back(instruction);
    }
    return instructions;
}
