/**
 * @file util.hpp
 * @brief Filesystem utility helpers for Rix I/O subsystem.
 *
 * Copyright 2026, Gaspard Kirira.
 * https://github.com/rixcpp/rix
 *
 * Use of this source code is governed by the MIT license.
 */

#ifndef RIX_IO_UTIL_HPP
#define RIX_IO_UTIL_HPP

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <random>
#include <string>
#include <string_view>
#include <system_error>

namespace rix::io
{
  /**
   * @brief Check whether a filesystem path exists.
   *
   * This function never throws.
   *
   * @param p Path to check.
   * @return true if the path exists, false otherwise.
   */
  [[nodiscard]] inline bool path_exists(const std::filesystem::path &p) noexcept
  {
    std::error_code ec;
    return std::filesystem::exists(p, ec);
  }

  /**
   * @brief Get the size of a regular file.
   *
   * @param p Path to the file.
   * @return File size in bytes.
   *
   * @throws std::filesystem::filesystem_error If the file does not exist
   *         or its size cannot be determined.
   */
  [[nodiscard]] inline std::uintmax_t path_size(const std::filesystem::path &p)
  {
    std::error_code ec;
    const auto size = std::filesystem::file_size(p, ec);

    if (ec)
    {
      throw std::filesystem::filesystem_error(
          "rix::io::path_size failed",
          p,
          ec);
    }

    return size;
  }

  /**
   * @brief Generate a temporary file path.
   *
   * The path is constructed inside the system temporary directory.
   * The file is not created.
   *
   * The generated name is composed of:
   * - prefix
   * - monotonic timestamp
   * - random value
   *
   * @param prefix Prefix for the file name.
   * @return A unique temporary path.
   *
   * @throws std::filesystem::filesystem_error If the system temporary directory
   *         cannot be determined.
   */
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

  /**
   * @deprecated Use `rix::io::path_exists()`.
   */
  [[deprecated("Use rix::io::path_exists().")]]
  inline bool exists(const std::filesystem::path &p) noexcept
  {
    return path_exists(p);
  }

  /**
   * @deprecated Use `rix::io::path_size()`.
   */
  [[deprecated("Use rix::io::path_size().")]]
  inline std::uintmax_t file_size(const std::filesystem::path &p)
  {
    return path_size(p);
  }

  /**
   * @deprecated Use `rix::io::temp_path()`.
   */
  [[deprecated("Use rix::io::temp_path().")]]
  inline std::filesystem::path temp_file_path(std::string_view prefix = "rix")
  {
    return temp_path(prefix);
  }

} // namespace rix::io

#endif // RIX_IO_UTIL_HPP
