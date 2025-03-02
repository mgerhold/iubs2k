#pragma once

#include <string_view>
#include "error.hpp"
#include <memory>
#include <common/instruction.hpp>

namespace assembler {
    class Parser;

    class Assembler final {
    private:
        std::unique_ptr<Parser> m_parser;

    public:
        [[nodiscard]] explicit Assembler(std::string_view filename, std::string_view source);

        Assembler(Assembler const& other) = delete;
        Assembler(Assembler&& other) noexcept = default;
        Assembler& operator=(Assembler const& other) = delete;
        Assembler& operator=(Assembler&& other) noexcept = default;
        ~Assembler();

        [[nodiscard]] tl::expected<::Instruction, Error> next_instruction();
    };
}
