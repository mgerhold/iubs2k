add_executable(
        main
        main.cpp
)

target_link_libraries(
        main
        PRIVATE
        emulator
        assembler
        gui
)
