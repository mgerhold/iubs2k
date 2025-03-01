#pragma once

#include <iostream>
#include <lib2k/types.hpp>
#include <magic_enum.hpp>

enum class Register : u8 {
    A,
    B,
    C,
    D,
};

inline auto format_as(Register const register_) {
    return magic_enum::enum_name(register_);
}
