/**
 * @file writer.hpp
 * @brief Convenience helpers for writing entire files as text or bytes.
 *
 * Copyright 2026, Gaspard Kirira.
 * https://github.com/rixcpp/rix
 *
 * Use of this source code is governed by the MIT license.
 */

#ifndef RIX_IO_WRITER_HPP
#define RIX_IO_WRITER_HPP

#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>

#include <rix/io/file.hpp>

namespace rix::io
{
  /**
   * @brief Write policy for file output.
   */
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

  /**
   * @brief Write text to a file.
   *
   * Opens the file with `FileType::text` and the requested write mode, writes all bytes,
   * and flushes the stream.
   *
   * @param path Path to the file.
   * @param text Text to write.
   * @param mode Write mode (truncate or append).
   *
   * @throws std::system_error If opening fails.
   * @throws std::runtime_error If writing or flushing fails.
   */
  inline void write_file_text(const std::filesystem::path &path,
                              std::string_view text,
                              WriteMode mode = WriteMode::truncate)
  {
    File f{path, detail::to_file_mode(mode), FileType::text};
    f.write(text);
    f.flush();
  }

  /**
   * @brief Write bytes to a file.
   *
   * Opens the file with `FileType::binary` and the requested write mode, writes all bytes,
   * and flushes the stream.
   *
   * @param path Path to the file.
   * @param bytes Bytes to write.
   * @param mode Write mode (truncate or append).
   *
   * @throws std::system_error If opening fails.
   * @throws std::runtime_error If writing or flushing fails.
   */
  inline void write_file_binary(const std::filesystem::path &path,
                                std::span<const std::byte> bytes,
                                WriteMode mode = WriteMode::truncate)
  {
    File f{path, detail::to_file_mode(mode), FileType::binary};
    f.write(bytes);
    f.flush();
  }

  /**
   * @deprecated Use `rix::io::write_file_text()`.
   */
  [[deprecated("Use rix::io::write_file_text().")]]
  inline void write_text(const std::filesystem::path &path,
                         std::string_view text,
                         WriteMode mode = WriteMode::truncate)
  {
    write_file_text(path, text, mode);
  }

  /**
   * @deprecated Use `rix::io::write_file_binary()`.
   */
  [[deprecated("Use rix::io::write_file_binary().")]]
  inline void write_binary(const std::filesystem::path &path,
                           std::span<const std::byte> bytes,
                           WriteMode mode = WriteMode::truncate)
  {
    write_file_binary(path, bytes, mode);
  }

} // namespace rix::io

#endif // RIX_IO_WRITER_HPP
