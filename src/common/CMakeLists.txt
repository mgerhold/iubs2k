add_library(
        common
        include/common/common.hpp
        include/common/instruction.hpp
        instruction.cpp
        include/common/opcode.hpp
        include/common/register.hpp
        include/common/pointer.hpp
)

target_include_directories(
        common
        PUBLIC
        include
)

target_link_system_libraries(
        common
        PUBLIC
        magic_enum::magic_enum
        lib2k
        fmt::fmt
)
