#pragma once

#include <lib2k/types.hpp>

enum class Opcode : u8 {
    HaltAndCatchFire,
    MoveImmediateIntoRegister,
    MoveImmediateIntoMemory,
};
