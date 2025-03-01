add_library(
        gui
        include/gui/gui.hpp
        gui.cpp
)

target_include_directories(gui PUBLIC include)

target_link_libraries(
        gui
        PRIVATE
        common
        emulator
)

target_link_system_libraries(
        gui
        PUBLIC
        lib2k
        SFML::Graphics
)
