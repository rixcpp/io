/**
 * @file reader.hpp
 * @brief Convenience helpers for reading entire files as text or bytes.
 *
 * Copyright 2026, Gaspard Kirira.
 * https://github.com/rixcpp/rix
 *
 * Use of this source code is governed by the MIT license.
 */

#ifndef RIX_IO_READER_HPP
#define RIX_IO_READER_HPP

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <rix/io/file.hpp>

namespace rix::io
{
  /**
   * @brief Read an entire file as text.
   *
   * Opens the file with `FileMode::read` and `FileType::text`.
   *
   * @param path Path to the file.
   * @return File content as a string.
   * @throws std::system_error If opening fails.
   * @throws std::runtime_error If reading fails.
   */
  [[nodiscard]] inline std::string read_file_text(const std::filesystem::path &path)
  {
    File f{path, FileMode::read, FileType::text};
    return f.read_all_text();
  }

  /**
   * @brief Read an entire file as bytes.
   *
   * Opens the file with `FileMode::read` and `FileType::binary`.
   *
   * @param path Path to the file.
   * @return File content as raw bytes.
   * @throws std::system_error If opening fails.
   * @throws std::runtime_error If reading fails.
   */
  [[nodiscard]] inline std::vector<std::byte> read_file_binary(const std::filesystem::path &path)
  {
    File f{path, FileMode::read, FileType::binary};
    return f.read_all_bytes();
  }

  /**
   * @brief Try to read an entire file as text.
   *
   * This function never throws. On any error it returns `std::nullopt`.
   *
   * @param path Path to the file.
   * @return `std::optional` containing file content on success, `std::nullopt` on failure.
   */
  [[nodiscard]] inline std::optional<std::string> try_read_file_text(const std::filesystem::path &path) noexcept
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

  /**
   * @brief Try to read an entire file as bytes.
   *
   * This function never throws. On any error it returns `std::nullopt`.
   *
   * @param path Path to the file.
   * @return `std::optional` containing file bytes on success, `std::nullopt` on failure.
   */
  [[nodiscard]] inline std::optional<std::vector<std::byte>> try_read_file_binary(const std::filesystem::path &path) noexcept
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

  /**
   * @deprecated Use `rix::io::read_file_text()`.
   */
  [[deprecated("Use rix::io::read_file_text().")]] [[nodiscard]] inline std::string read_text(const std::filesystem::path &path)
  {
    return read_file_text(path);
  }

  /**
   * @deprecated Use `rix::io::read_file_binary()`.
   */
  [[deprecated("Use rix::io::read_file_binary().")]] [[nodiscard]] inline std::vector<std::byte> read_binary(const std::filesystem::path &path)
  {
    return read_file_binary(path);
  }

  /**
   * @deprecated Use `rix::io::try_read_file_text()`.
   */
  [[deprecated("Use rix::io::try_read_file_text().")]] [[nodiscard]] inline std::optional<std::string> try_read_text(const std::filesystem::path &path) noexcept
  {
    return try_read_file_text(path);
  }

  /**
   * @deprecated Use `rix::io::try_read_file_binary()`.
   */
  [[deprecated("Use rix::io::try_read_file_binary().")]] [[nodiscard]] inline std::optional<std::vector<std::byte>> try_read_binary(const std::filesystem::path &path) noexcept
  {
    return try_read_file_binary(path);
  }

} // namespace rix::io

#endif // RIX_IO_READER_HPP
