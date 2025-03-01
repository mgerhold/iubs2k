#pragma once

#include <fmt/format.h>
#include "register.hpp"

class Pointer final {
private:
    Register m_register;

public:
    [[nodiscard]] explicit Pointer(Register const register_)
        : m_register{ register_ } {}

    [[nodiscard]] Register register_() const {
        return m_register;
    }

    [[nodiscard]] static constexpr usize byte_length() {
        return sizeof(m_register);
    }
};

[[nodiscard]] inline Pointer operator*(Register const register_) {
    return Pointer{ register_ };
}

[[nodiscard]] inline auto format_as(Pointer const pointer) {
    return fmt::format("*{}", pointer.register_());
}
