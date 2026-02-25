/**
 * @file file.hpp
 * @brief RAII file wrapper with explicit access mode and text/binary type.
 *
 * Copyright 2026, Gaspard Kirira.
 * https://github.com/rixcpp/rix
 *
 * Use of this source code is governed by the MIT license.
 */

#ifndef RIX_IO_FILE_HPP
#define RIX_IO_FILE_HPP

#include <cerrno>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace rix::io
{
  /**
   * @brief File open mode.
   */
  enum class FileMode
  {
    read,
    write,
    append,
    read_write
  };

  /**
   * @brief File content type.
   *
   * `binary` sets `std::ios_base::binary` on the underlying stream.
   */
  enum class FileType
  {
    text,
    binary
  };

  namespace detail
  {
    inline std::ios_base::openmode to_openmode(FileMode mode, FileType type)
    {
      std::ios_base::openmode m{};

      switch (mode)
      {
      case FileMode::read:
        m |= std::ios_base::in;
        break;
      case FileMode::write:
        m |= std::ios_base::out | std::ios_base::trunc;
        break;
      case FileMode::append:
        m |= std::ios_base::out | std::ios_base::app;
        break;
      case FileMode::read_write:
        m |= std::ios_base::in | std::ios_base::out;
        break;
      }

      if (type == FileType::binary)
      {
        m |= std::ios_base::binary;
      }

      return m;
    }

    [[nodiscard]] inline bool mode_can_read(FileMode m) noexcept
    {
      return (m == FileMode::read || m == FileMode::read_write);
    }

    [[nodiscard]] inline bool mode_can_write(FileMode m) noexcept
    {
      return (m == FileMode::write || m == FileMode::append || m == FileMode::read_write);
    }
  } // namespace detail

  /**
   * @brief RAII wrapper around `std::fstream` with explicit mode checks.
   *
   * Opens a file on construction and closes it on destruction.
   *
   * Error reporting:
   * - Open errors: `std::system_error` with `errno`
   * - Operation errors: `std::runtime_error`
   */
  class File
  {
  public:
    /**
     * @brief Construct a closed file handle.
     */
    File() = default;

    /**
     * @brief Open a file at `path` using `mode` and `type`.
     *
     * @throws std::system_error if opening fails.
     */
    File(const std::filesystem::path &path, FileMode mode, FileType type = FileType::text)
        : path_(path), mode_(mode), type_(type)
    {
      open_internal();
    }

    /**
     * @brief Close the file if open.
     *
     * Never throws.
     */
    ~File() noexcept { close(); }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

    /**
     * @brief Move-construct.
     */
    File(File &&other) noexcept
        : path_(std::move(other.path_)),
          stream_(std::move(other.stream_)),
          mode_(other.mode_),
          type_(other.type_)
    {
      other.path_.clear();
    }

    /**
     * @brief Move-assign.
     */
    File &operator=(File &&other) noexcept
    {
      if (this != &other)
      {
        close();
        path_ = std::move(other.path_);
        stream_ = std::move(other.stream_);
        mode_ = other.mode_;
        type_ = other.type_;
        other.path_.clear();
      }
      return *this;
    }

    /**
     * @brief Whether the underlying stream is open.
     */
    [[nodiscard]] bool is_open() const noexcept { return stream_.is_open(); }

    /**
     * @brief Convenience boolean conversion (same as `is_open()`).
     */
    explicit operator bool() const noexcept { return is_open(); }

    /**
     * @brief Path associated with this file handle.
     */
    [[nodiscard]] const std::filesystem::path &path() const noexcept { return path_; }

    /**
     * @brief Mode used for opening.
     */
    [[nodiscard]] FileMode mode() const noexcept { return mode_; }

    /**
     * @brief Type used for opening.
     */
    [[nodiscard]] FileType type() const noexcept { return type_; }

    /**
     * @brief Close the file if open.
     *
     * Never throws.
     */
    void close() noexcept
    {
      if (stream_.is_open())
      {
        try
        {
          stream_.close();
        }
        catch (...)
        {
        }
      }
    }

    /**
     * @brief Read entire file as text.
     *
     * The stream is rewound before reading.
     *
     * @throws std::runtime_error if the file is not open or not readable, or if reading fails.
     */
    [[nodiscard]] std::string read_all_text()
    {
      require_open();
      require_readable();

      stream_.clear();
      stream_.seekg(0, std::ios::beg);

      std::string content;
      content.assign(std::istreambuf_iterator<char>(stream_), std::istreambuf_iterator<char>());

      if (!stream_.eof() && stream_.fail())
      {
        throw std::runtime_error("rix::io::File: read_all_text failed: " + path_.string());
      }

      return content;
    }

    /**
     * @brief Read entire file as bytes.
     *
     * The stream is rewound before reading.
     *
     * @throws std::runtime_error if the file is not open or not readable, or if reading fails.
     */
    [[nodiscard]] std::vector<std::byte> read_all_bytes()
    {
      require_open();
      require_readable();

      stream_.clear();
      stream_.seekg(0, std::ios::end);
      const std::streampos end = stream_.tellg();
      stream_.seekg(0, std::ios::beg);
      const std::streampos begin = stream_.tellg();

      if (end < begin)
      {
        throw std::runtime_error("rix::io::File: invalid size: " + path_.string());
      }

      const auto size = static_cast<std::size_t>(end - begin);
      std::vector<std::byte> buffer(size);

      if (size != 0)
      {
        stream_.read(reinterpret_cast<char *>(buffer.data()), static_cast<std::streamsize>(size));

        if (static_cast<std::size_t>(stream_.gcount()) != size || stream_.fail())
        {
          throw std::runtime_error("rix::io::File: read_all_bytes failed: " + path_.string());
        }
      }

      return buffer;
    }

    /**
     * @brief Write text to file.
     *
     * @throws std::runtime_error if the file is not open or not writable, or if writing fails.
     */
    void write(std::string_view text)
    {
      require_open();
      require_writable();

      stream_.write(text.data(), static_cast<std::streamsize>(text.size()));
      if (!stream_)
      {
        throw std::runtime_error("rix::io::File: write(text) failed: " + path_.string());
      }
    }

    /**
     * @brief Write bytes to file.
     *
     * @throws std::runtime_error if the file is not open or not writable, or if writing fails.
     */
    void write(std::span<const std::byte> bytes)
    {
      require_open();
      require_writable();

      if (!bytes.empty())
      {
        stream_.write(reinterpret_cast<const char *>(bytes.data()),
                      static_cast<std::streamsize>(bytes.size()));
      }

      if (!stream_)
      {
        throw std::runtime_error("rix::io::File: write(bytes) failed: " + path_.string());
      }
    }

    /**
     * @brief Flush the underlying stream.
     *
     * @throws std::runtime_error if flushing fails.
     */
    void flush()
    {
      require_open();

      stream_.flush();
      if (!stream_)
      {
        throw std::runtime_error("rix::io::File: flush failed: " + path_.string());
      }
    }

  private:
    std::filesystem::path path_{};
    std::fstream stream_{};
    FileMode mode_{FileMode::read};
    FileType type_{FileType::text};

    void open_internal()
    {
      stream_.open(path_, detail::to_openmode(mode_, type_));

      if (!stream_.is_open())
      {
        throw std::system_error(errno, std::generic_category(), "rix::io::File: open failed: " + path_.string());
      }
    }

    void require_open() const
    {
      if (!stream_.is_open())
      {
        throw std::runtime_error("rix::io::File: not open: " + path_.string());
      }
    }

    void require_readable() const
    {
      if (!detail::mode_can_read(mode_))
      {
        throw std::runtime_error("rix::io::File: not readable: " + path_.string());
      }
    }

    void require_writable() const
    {
      if (!detail::mode_can_write(mode_))
      {
        throw std::runtime_error("rix::io::File: not writable: " + path_.string());
      }
    }
  };

} // namespace rix::io

#endif // RIX_IO_FILE_HPP
