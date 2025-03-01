add_library(
        emulator
        include/emulator/emulator.hpp
        emulator.cpp
        include/emulator/memory_mapped_device.hpp
        include/emulator/text_device.hpp
)

target_include_directories(
        emulator
        PUBLIC
        include
)

target_link_libraries(
        emulator
        PUBLIC
        common
)

target_link_system_libraries(
        emulator
        PUBLIC
        lib2k
        magic_enum::magic_enum
        tl::optional
        tl::expected
        spdlog::spdlog
)
