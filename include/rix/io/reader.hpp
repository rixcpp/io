#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <cstddef>

#include "rix/io/file.hpp"

namespace rix::io
{

    inline std::string read_text(const std::filesystem::path &path)
    {
        File f{path, FileMode::Read, FileType::Text};
        return f.read_all_text();
    }

    inline std::vector<std::byte> read_binary(const std::filesystem::path &path)
    {
        File f{path, FileMode::Read, FileType::Binary};
        return f.read_all_bytes();
    }

    inline std::optional<std::string> try_read_text(const std::filesystem::path &path) noexcept
    {
        try
        {
            return read_text(path);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    inline std::optional<std::vector<std::byte>> try_read_binary(const std::filesystem::path &path) noexcept
    {
        try
        {
            return read_binary(path);
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

} // namespace rix::io
