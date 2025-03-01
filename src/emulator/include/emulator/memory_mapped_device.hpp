#pragma once

#include <concepts>
#include <lib2k/types.hpp>

namespace detail {
    template<usize>
    struct NonTypeTemplateParameter {};
}  // namespace detail

template<typename T>
concept MemoryMappedDevice = requires {
    // num_mapped_bytes must be a constant expression of type usize.
    T::num_mapped_bytes;
    requires std::same_as<usize const, decltype(T::num_mapped_bytes)>;
    typename ::detail::NonTypeTemplateParameter<T::num_mapped_bytes>;

    // alignment must be a constant expression of type usize.
    T::alignment;
    requires std::same_as<usize const, decltype(T::alignment)>;
    typename ::detail::NonTypeTemplateParameter<T::alignment>;
};
