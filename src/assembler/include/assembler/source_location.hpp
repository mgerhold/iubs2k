#pragma once

#include <fmt/format.h>
#include <lib2k/types.hpp>
#include <string_view>

namespace assembler {
    class SourceLocation final {
    private:
        std::string_view m_filename;
        std::string_view m_source;
        usize m_offset;
        usize m_length;
        usize m_row;
        usize m_column;

    public:
        [[nodiscard]] SourceLocation(
            std::string_view filename,
            std::string_view source,
            usize const offset,
            usize const length,
            usize const row,
            usize const column
        )
            : m_filename{ std::move(filename) },
              m_source{ std::move(source) },
              m_offset{ offset },
              m_length{ length },
              m_row{ row },
              m_column{ column } {}

        [[nodiscard]] std::string_view filename() const {
            return m_filename;
        }

        [[nodiscard]] std::string_view lexeme() const {
            return m_source.substr(m_offset, m_length);
        }

        [[nodiscard]] usize row() const {
            return m_row;
        }

        [[nodiscard]] usize column() const {
            return m_column;
        }
    };

    [[nodiscard]] inline std::string format_as(SourceLocation const& location) {
        return fmt::format("{}:{}:{}", location.filename(), location.row(), location.column());
    }
}  // namespace assembler
