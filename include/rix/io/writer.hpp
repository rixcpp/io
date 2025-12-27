#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>

#include "rix/io/file.hpp"

namespace rix::io
{
    enum class WriteMode
    {
        truncate,
        append
    };

    namespace detail
    {
        [[nodiscard]] inline constexpr FileMode to_file_mode(WriteMode mode) noexcept
        {
            return (mode == WriteMode::append) ? FileMode::append : FileMode::write;
        }
    } // namespace detail

    inline void write_file_text(const std::filesystem::path &path,
                                std::string_view text,
                                WriteMode mode = WriteMode::truncate)
    {
        File f{path, detail::to_file_mode(mode), FileType::text};
        f.write(text);
        f.flush();
    }

    inline void write_file_binary(const std::filesystem::path &path,
                                  std::span<const std::byte> bytes,
                                  WriteMode mode = WriteMode::truncate)
    {
        File f{path, detail::to_file_mode(mode), FileType::binary};
        f.write(bytes);
        f.flush();
    }

    // Deprecated aliases (transition only)
    [[deprecated("Use rix::io::write_file_text()")]]
    inline void write_text(const std::filesystem::path &path,
                           std::string_view text,
                           WriteMode mode = WriteMode::truncate)
    {
        write_file_text(path, text, mode);
    }

    [[deprecated("Use rix::io::write_file_binary()")]]
    inline void write_binary(const std::filesystem::path &path,
                             std::span<const std::byte> bytes,
                             WriteMode mode = WriteMode::truncate)
    {
        write_file_binary(path, bytes, mode);
    }

} // namespace rix::io
