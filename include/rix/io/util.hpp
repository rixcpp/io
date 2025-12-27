#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <random>
#include <string>
#include <string_view>
#include <system_error>

namespace rix::io
{
    [[nodiscard]] inline bool path_exists(const std::filesystem::path &p) noexcept
    {
        std::error_code ec;
        return std::filesystem::exists(p, ec);
    }

    [[nodiscard]] inline std::uintmax_t path_size(const std::filesystem::path &p)
    {
        std::error_code ec;
        const auto size = std::filesystem::file_size(p, ec);
        if (ec)
        {
            throw std::filesystem::filesystem_error(
                "path_size failed",
                p,
                ec);
        }
        return size;
    }

    [[nodiscard]] inline std::filesystem::path temp_path(std::string_view prefix = "rix")
    {
        const auto base = std::filesystem::temp_directory_path();

        const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937_64 rng(static_cast<std::mt19937_64::result_type>(now));
        const auto rand_val = rng();

        std::string name;
        name.reserve(prefix.size() + 32);
        name.append(prefix);
        name.push_back('_');
        name.append(std::to_string(now));
        name.push_back('_');
        name.append(std::to_string(rand_val));
        name.append(".tmp");

        return base / name;
    }

    [[deprecated("Use rix::io::path_exists()")]]
    inline bool exists(const std::filesystem::path &p) noexcept
    {
        return path_exists(p);
    }

    [[deprecated("Use rix::io::path_size()")]]
    inline std::uintmax_t file_size(const std::filesystem::path &p)
    {
        return path_size(p);
    }

    [[deprecated("Use rix::io::temp_path()")]]
    inline std::filesystem::path temp_file_path(std::string_view prefix = "rix")
    {
        return temp_path(prefix);
    }

} // namespace rix::io
