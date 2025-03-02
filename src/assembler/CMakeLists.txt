add_library(
        assembler
        include/assembler/assembler.hpp
        assembler.cpp
        include/assembler/error.hpp
        parser.hpp
        lexer.hpp
        parser.cpp
        lexer.cpp
        include/assembler/token.hpp
        include/assembler/source_location.hpp
        include/assembler/instruction.hpp
        include/assembler/operand.hpp
        instruction.cpp
)

target_include_directories(
        assembler
        PUBLIC
        include
)

target_link_libraries(
        assembler
        PUBLIC
        common
)

target_link_system_libraries(
        assembler
        PRIVATE
        ctre::ctre
)
