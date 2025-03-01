#pragma once

#include <fmt/format.h>
#include <span>
#include <stdexcept>
#include <string>
#include "memory_mapped_device.hpp"

class TextDevice final {
public:
    static constexpr auto num_rows = usize{ 24 };
    static constexpr auto num_columns = usize{ 80 };
    static constexpr auto num_mapped_bytes = usize{ num_rows * num_columns };
    static constexpr auto alignment = usize{ 1 };

private:
    std::span<std::byte> m_mapped_memory;

public:
    [[nodiscard]] explicit TextDevice(std::span<std::byte> const mapped_memory)
        : m_mapped_memory{ mapped_memory } {
        if (m_mapped_memory.size() != num_mapped_bytes) {
            throw std::runtime_error{ "Invalid mapped memory size." };
        }
    }

    [[nodiscard]] char operator[](usize const x, usize const y) const {
        auto const index = x + y * num_columns;
        if (index >= num_mapped_bytes) {
            throw std::out_of_range{ fmt::format("Invalid index: ({}, {})", x, y) };
        }
        return std::to_integer<char>(m_mapped_memory[index]);
    }

    [[nodiscard]] std::string text() const {
        auto result = std::string{};
        result.reserve(num_rows * num_columns + (num_rows - 1));
        for (auto row = usize{ 0 }; row < num_rows; ++row) {
            auto const line = std::string_view{
                reinterpret_cast<char const*>(&m_mapped_memory[row * num_columns]),
                num_columns,
            };
            result += line;
            if (row != num_rows - 1) {
                result += '\n';
            }
        }
        return result;
    }
};

static_assert(MemoryMappedDevice<TextDevice>);
