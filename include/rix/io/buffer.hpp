/**
 * @file buffer.hpp
 * @brief Owning contiguous byte buffer for Rix I/O subsystem.
 *
 * Copyright 2026, Gaspard Kirira.
 * https://github.com/rixcpp/rix
 *
 * Use of this source code is governed by the MIT license.
 */

#ifndef RIX_IO_BUFFER_HPP
#define RIX_IO_BUFFER_HPP

#include <cstddef>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace rix::io
{
  /**
   * @brief Owning contiguous byte buffer.
   *
   * Provides:
   * - Span-based access
   * - Text helpers (no UTF-8 validation)
   * - POD read/write helpers (native endianness)
   *
   * No implicit encoding or endian conversion is performed.
   */
  class Buffer
  {
  public:
    using value_type = std::byte;
    using storage_type = std::vector<std::byte>;
    using size_type = std::size_t;

    Buffer() = default;

    explicit Buffer(size_type size)
        : data_(size)
    {
    }

    explicit Buffer(const storage_type &bytes)
        : data_(bytes)
    {
    }

    explicit Buffer(storage_type &&bytes) noexcept
        : data_(std::move(bytes))
    {
    }

    explicit Buffer(std::string_view text) { assign(text); }

    explicit Buffer(std::span<const std::byte> bytes) { assign(bytes); }

    [[nodiscard]] size_type size() const noexcept { return data_.size(); }

    [[nodiscard]] bool empty() const noexcept { return data_.empty(); }

    [[nodiscard]] std::byte *data() noexcept { return data_.data(); }

    [[nodiscard]] const std::byte *data() const noexcept { return data_.data(); }

    [[nodiscard]] std::span<std::byte> span() noexcept
    {
      return std::span<std::byte>(data_.data(), data_.size());
    }

    [[nodiscard]] std::span<const std::byte> span() const noexcept
    {
      return std::span<const std::byte>(data_.data(), data_.size());
    }

    [[nodiscard]] const storage_type &bytes() const noexcept { return data_; }

    [[nodiscard]] storage_type &bytes() noexcept { return data_; }

    /**
     * @brief Unchecked byte access.
     */
    [[nodiscard]] std::byte &operator[](size_type i) noexcept { return data_[i]; }

    /**
     * @brief Unchecked byte access.
     */
    [[nodiscard]] const std::byte &operator[](size_type i) const noexcept { return data_[i]; }

    /**
     * @brief Checked byte access.
     *
     * @throws std::out_of_range if index is invalid.
     */
    [[nodiscard]] std::byte &at(size_type i) { return data_.at(i); }

    /**
     * @brief Checked byte access.
     *
     * @throws std::out_of_range if index is invalid.
     */
    [[nodiscard]] const std::byte &at(size_type i) const { return data_.at(i); }

    /**
     * @brief Zero-copy view as string bytes.
     *
     * Safe only if the underlying payload represents text.
     */
    [[nodiscard]] std::string_view as_string_view() const noexcept
    {
      const auto *p = reinterpret_cast<const char *>(data_.data());
      return std::string_view(p, data_.size());
    }

    /**
     * @brief Copy buffer content into a std::string.
     */
    [[nodiscard]] std::string to_string() const
    {
      return std::string(as_string_view());
    }

    void clear() noexcept { data_.clear(); }

    /**
     * @brief Clear and release memory.
     */
    void reset() noexcept
    {
      storage_type tmp;
      data_.swap(tmp);
    }

    void resize(size_type size) { data_.resize(size); }

    void reserve(size_type cap) { data_.reserve(cap); }

    void shrink_to_fit() { data_.shrink_to_fit(); }

    void swap(Buffer &other) noexcept { data_.swap(other.data_); }

    void assign(std::string_view text)
    {
      data_.resize(text.size());
      if (!text.empty())
      {
        std::memcpy(data_.data(), text.data(), text.size());
      }
    }

    void assign(std::span<const std::byte> bytes)
    {
      data_.assign(bytes.begin(), bytes.end());
    }

    void append(std::string_view text)
    {
      if (text.empty())
      {
        return;
      }

      const auto old = data_.size();
      data_.resize(old + text.size());
      std::memcpy(data_.data() + old, text.data(), text.size());
    }

    void append(std::span<const std::byte> bytes)
    {
      if (bytes.empty())
      {
        return;
      }

      data_.insert(data_.end(), bytes.begin(), bytes.end());
    }

    /**
     * @brief Append a single byte.
     */
    void push_back(std::byte b) { data_.push_back(b); }

    /**
     * @brief Append a single byte from an unsigned char.
     */
    void append_byte(unsigned char b) { data_.push_back(static_cast<std::byte>(b)); }

    /**
     * @brief Interpret buffer data as a pointer to `T`.
     *
     * @return Pointer to T, or nullptr if buffer is empty.
     */
    template <class T>
    [[nodiscard]] T *data_as() noexcept
    {
      static_assert(std::is_trivially_copyable_v<T>, "data_as requires trivially copyable type");
      return data_.empty() ? nullptr : reinterpret_cast<T *>(data_.data());
    }

    /**
     * @brief Interpret buffer data as a pointer to `T`.
     *
     * @return Pointer to T, or nullptr if buffer is empty.
     */
    template <class T>
    [[nodiscard]] const T *data_as() const noexcept
    {
      static_assert(std::is_trivially_copyable_v<T>, "data_as requires trivially copyable type");
      return data_.empty() ? nullptr : reinterpret_cast<const T *>(data_.data());
    }

    /**
     * @brief Append a trivially copyable value as raw bytes.
     *
     * Stored in native endianness.
     */
    template <class T>
    void append_pod(const T &value)
    {
      static_assert(std::is_trivially_copyable_v<T>, "append_pod requires trivially copyable type");

      const auto old = data_.size();
      data_.resize(old + sizeof(T));
      std::memcpy(data_.data() + old, &value, sizeof(T));
    }

    /**
     * @brief Read a trivially copyable value from the buffer.
     *
     * @throws std::invalid_argument if out of range.
     */
    template <class T>
    [[nodiscard]] T read_pod(size_type offset) const
    {
      static_assert(std::is_trivially_copyable_v<T>, "read_pod requires trivially copyable type");

      if (offset > size() || (size() - offset) < sizeof(T))
      {
        throw std::invalid_argument("rix::io::Buffer::read_pod: out of range");
      }

      T out{};
      std::memcpy(&out, data_.data() + offset, sizeof(T));
      return out;
    }

    /**
     * @brief Overwrite a trivially copyable value at offset.
     *
     * @throws std::invalid_argument if out of range.
     */
    template <class T>
    void write_pod(size_type offset, const T &value)
    {
      static_assert(std::is_trivially_copyable_v<T>, "write_pod requires trivially copyable type");

      if (offset > size() || (size() - offset) < sizeof(T))
      {
        throw std::invalid_argument("rix::io::Buffer::write_pod: out of range");
      }

      std::memcpy(data_.data() + offset, &value, sizeof(T));
    }

  private:
    storage_type data_{};
  };

} // namespace rix::io

#endif // RIX_IO_BUFFER_HPP
