#pragma once

#include <array>
#include <common/common.hpp>
#include <common/pointer.hpp>
#include <common/register.hpp>
#include <cstddef>
#include <cstdlib>
#include <lib2k/types.hpp>
#include <magic_enum.hpp>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>
#include "text_device.hpp"

class Emulator final {
private:
    std::vector<std::byte> m_memory;
    std::size_t m_instruction_pointer = 0;
    bool m_is_halted = false;
    std::array<Word, magic_enum::enum_count<Register>()> m_registers{};
    TextDevice m_text_device;

public:
    [[nodiscard]] explicit Emulator(std::span<std::byte const> memory);

    Emulator(Emulator const& other) = delete;
    Emulator(Emulator&& other) noexcept = delete;
    Emulator& operator=(Emulator const& other) = delete;
    Emulator& operator=(Emulator&& other) noexcept = delete;
    ~Emulator() = default;

    void step();

    [[nodiscard]] bool is_halted() const {
        return m_is_halted;
    }

    [[nodiscard]] Word read_register(Register const which) const {
        return m_registers.at(std::to_underlying(which));
    }

    void write_register(Register const which, Word const value) {
        m_registers.at(std::to_underlying(which)) = value;
    }

    void write_into_memory(Pointer const pointer, Word const value) {
        auto const address = read_register(pointer.register_());
        auto const begin = reinterpret_cast<std::byte const*>(&value);
        auto const end = begin + sizeof(value);
        std::copy(begin, end, m_memory.begin() + address);
    }

    [[nodiscard]] TextDevice const& text_device() const {
        return m_text_device;
    }
};
