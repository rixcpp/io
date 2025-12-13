#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <span>
#include <cstddef>

#include "rix/io/file.hpp"

namespace rix::io
{

    enum class WriteMode
    {
        Truncate, // overwrite existing file
        Append    // append to file
    };

    /// Write text to file (truncate or append). Throws on error.
    inline void write_text(const std::filesystem::path &path,
                           std::string_view text,
                           WriteMode mode = WriteMode::Truncate)
    {
        FileMode fmode = (mode == WriteMode::Append)
                             ? FileMode::Append
                             : FileMode::Write;

        File f{path, fmode, FileType::Text};
        f.write(text);
        f.flush();
    }

    /// Write raw bytes to file (truncate or append). Throws on error.
    inline void write_binary(const std::filesystem::path &path,
                             std::span<const std::byte> bytes,
                             WriteMode mode = WriteMode::Truncate)
    {
        FileMode fmode = (mode == WriteMode::Append)
                             ? FileMode::Append
                             : FileMode::Write;

        File f{path, fmode, FileType::Binary};
        f.write(bytes);
        f.flush();
    }

} // namespace rix::io
