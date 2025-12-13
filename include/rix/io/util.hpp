#pragma once

#include <filesystem>
#include <chrono>
#include <random>
#include <string>
#include <string_view>
#include <cstdint>

namespace rix::io
{

    inline bool exists(const std::filesystem::path &p) noexcept
    {
        std::error_code ec;
        return std::filesystem::exists(p, ec);
    }

    inline std::uintmax_t file_size(const std::filesystem::path &p)
    {
        std::error_code ec;
        auto size = std::filesystem::file_size(p, ec);
        if (ec)
        {
            throw std::filesystem::filesystem_error(
                "file_size failed",
                p,
                ec);
        }
        return size;
    }

    inline std::filesystem::path temp_file_path(std::string_view prefix = "rix")
    {
        auto base = std::filesystem::temp_directory_path();

        auto now = std::chrono::steady_clock::now().time_since_epoch().count();
        std::mt19937_64 rng(static_cast<std::mt19937_64::result_type>(now));
        auto rand_val = rng();

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

} // namespace rix::io
