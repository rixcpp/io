#pragma once

#include <cstddef>    // std::byte
#include <filesystem> // std::filesystem::path
#include <optional>   // std::optional
#include <string>     // std::string
#include <vector>     // std::vector

#include "rix/io/file.hpp"

namespace rix::io
{
    inline std::string read_file_text(const std::filesystem::path &path)
    {
        File f{path, FileMode::read, FileType::text};
        return f.read_all_text();
    }

    inline std::vector<std::byte> read_file_binary(const std::filesystem::path &path)
    {
        File f{path, FileMode::read, FileType::binary};
        return f.read_all_bytes();
    }

    inline std::optional<std::string> try_read_file_text(const std::filesystem::path &path) noexcept
    {
        try
        {
            return read_file_text(path);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    inline std::optional<std::vector<std::byte>> try_read_file_binary(const std::filesystem::path &path) noexcept
    {
        try
        {
            return read_file_binary(path);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    [[deprecated("Use rix::io::read_file_text()")]]
    inline std::string read_text(const std::filesystem::path &path)
    {
        return read_file_text(path);
    }

    [[deprecated("Use rix::io::read_file_binary()")]]
    inline std::vector<std::byte> read_binary(const std::filesystem::path &path)
    {
        return read_file_binary(path);
    }

    [[deprecated("Use rix::io::try_read_file_text()")]]
    inline std::optional<std::string> try_read_text(const std::filesystem::path &path) noexcept
    {
        return try_read_file_text(path);
    }

    [[deprecated("Use rix::io::try_read_file_binary()")]]
    inline std::optional<std::vector<std::byte>> try_read_binary(const std::filesystem::path &path) noexcept
    {
        return try_read_file_binary(path);
    }

} // namespace rix::io
