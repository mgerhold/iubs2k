#pragma once

#include <bit>
#include <concepts>
#include <lib2k/types.hpp>

using Word = u32;

template<std::integral Integral>
[[nodiscard]] constexpr Integral to_little_endian(Integral const integral) {
    if constexpr (std::endian::native == std::endian::little) {
        return integral;
    } else {
        return std::byteswap(integral);
    }
}

template<std::integral Integral>
[[nodiscard]] constexpr Integral from_little_endian(Integral const integral) {
    return to_little_endian(integral);
}
