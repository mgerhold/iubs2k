#pragma once

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <common/common.hpp>
#include <common/pointer.hpp>
#include <common/register.hpp>
#include <lib2k/static_vector.hpp>
#include <memory>
#include <variant>
#include <vector>
#include "opcode.hpp"

class Instruction;

struct HaltAndCatchFire final {
    static constexpr auto opcode = Opcode::HaltAndCatchFire;
    static constexpr auto byte_length = usize{ 1 };

    void encode_into(std::span<std::byte> buffer) const;

    [[nodiscard]] static Instruction decode(std::span<std::byte const> buffer);
};

struct MoveImmediateIntoRegister final {
    static constexpr auto opcode = Opcode::MoveImmediateIntoRegister;
    static constexpr auto byte_length = usize{ 1 + sizeof(Word) + 1 };  // Opcode + immediate + register.

    Word immediate;
    Register register_;

    [[nodiscard]] explicit MoveImmediateIntoRegister(Word const immediate, Register const register_)
        : immediate{ immediate }, register_{ register_ } {}

    void encode_into(std::span<std::byte> buffer) const;

    [[nodiscard]] static Instruction decode(std::span<std::byte const> buffer);

    void format(std::ostream& os) const {
        fmt::print(os, " - {} (0x{:08x}) into {}", immediate, immediate, register_);
    }
};

struct MoveImmediateIntoMemory final {
    static constexpr auto opcode = Opcode::MoveImmediateIntoMemory;
    static constexpr auto byte_length =
        usize{ 1 + sizeof(Word) + Pointer::byte_length() };  // Opcode + immediate + pointer.

    Word immediate;
    Pointer pointer;

    [[nodiscard]] explicit MoveImmediateIntoMemory(Word const immediate, Pointer pointer)
        : immediate{ immediate }, pointer{ pointer } {}

    void encode_into(std::span<std::byte> buffer) const;

    [[nodiscard]] static Instruction decode(std::span<std::byte const> buffer);

    void format(std::ostream& os) const {
        fmt::print(os, " - {} (0x{:08x}) into {}", immediate, immediate, pointer);
    }
};

// clang-format off
using InstructionBase = std::variant<
    HaltAndCatchFire,
    MoveImmediateIntoRegister,
    MoveImmediateIntoMemory
>;
// clang-format on

class Instruction final : public InstructionBase {
private:
    template<typename T>
    struct Decoder;

    template<typename FirstVariant, typename... OtherVariants>
    struct Decoder<std::variant<FirstVariant, OtherVariants...>> {
        [[nodiscard]] static Instruction decode(Opcode const opcode, std::span<std::byte const> const buffer) {
            if (FirstVariant::opcode == opcode) {
                return FirstVariant::decode(buffer);
            }
            if constexpr (sizeof...(OtherVariants) == 0) {
                throw std::runtime_error{ "Not implemented." };
            } else {
                return Decoder<std::variant<OtherVariants...>>::decode(opcode, buffer);
            }
        }
    };

public:
    using variant::variant;

    [[nodiscard]] Opcode opcode() const {
        return std::visit([](auto const& instruction) { return instruction.opcode; }, *this);
    }

    [[nodiscard]] usize byte_length() const {
        return std::visit([](auto const& instruction) { return instruction.byte_length; }, *this);
    }

    void encode(std::output_iterator<std::byte> auto output) const {
        std::visit(
            [&]<typename Instruction>(Instruction const& instruction) {
                auto buffer = std::array<std::byte, Instruction::byte_length>{};
                instruction.encode_into(buffer);
                std::copy(buffer.begin(), buffer.end(), output);
            },
            *this
        );
    }

    [[nodiscard]] static Instruction decode(std::span<std::byte const> const buffer) {
        auto const opcode = magic_enum::enum_cast<Opcode>(std::to_integer<std::underlying_type_t<Opcode>>(buffer[0]));
        if (not opcode.has_value()) {
            throw std::runtime_error{ fmt::format("Invalid opcode: 0x{:x}", std::to_integer<int>(buffer[0])) };
        }
        return Decoder<variant>::decode(opcode.value(), buffer);
    }

    friend std::ostream& operator<<(std::ostream& os, Instruction const& instruction) {
        fmt::print(
            os,
            "0x{:02x} - {}",
            std::to_underlying(instruction.opcode()),
            magic_enum::enum_name(instruction.opcode())
        );
        std::visit(
            [&]<typename T>(T const& inst) {
                if constexpr (requires { inst.format(os); }) {
                    inst.format(os);
                }
            },
            instruction
        );
        return os;
    }
};

[[nodiscard]] std::vector<Instruction> decode(std::span<std::byte const> memory);

[[nodiscard]] inline auto format_as(Instruction const& instruction) {
    return fmt::format(
        "0x{:02x} - {}{}",
        std::to_underlying(instruction.opcode()),
        magic_enum::enum_name(instruction.opcode()),
        std::visit(
            [&](auto const& inst) {
                if constexpr (requires { format_as(inst); }) {
                    return format_as(inst);
                } else {
                    return "";
                }
            },
            instruction
        )
    );
}
