#pragma once

#include <cstddef>     // std::byte, std::size_t
#include <cstring>     // std::memcpy
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector
#include <cstring>

namespace rix::io
{
    class Buffer
    {
    public:
        Buffer() = default;

        explicit Buffer(std::size_t size)
            : data_(size)
        {
        }

        explicit Buffer(const std::vector<std::byte> &bytes)
            : data_(bytes)
        {
        }

        explicit Buffer(std::vector<std::byte> &&bytes) noexcept
            : data_(std::move(bytes))
        {
        }

        /// Construct from text (UTF-8 assumed) – stored as bytes.
        explicit Buffer(std::string_view text)
        {
            assign(text);
        }

        // --------------------------- Observers ---------------------------
        [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }
        [[nodiscard]] bool empty() const noexcept { return data_.empty(); }

        [[nodiscard]] std::byte *data() noexcept { return data_.data(); }
        [[nodiscard]] const std::byte *data() const noexcept { return data_.data(); }

        [[nodiscard]] std::span<std::byte> span() noexcept { return {data_.data(), data_.size()}; }
        [[nodiscard]] std::span<const std::byte> span() const noexcept { return {data_.data(), data_.size()}; }

        [[nodiscard]] std::span<const std::byte> bytes_view() const noexcept { return span(); }
        [[nodiscard]] std::span<std::byte> bytes_view() noexcept { return span(); }

        [[nodiscard]] const std::vector<std::byte> &bytes() const noexcept { return data_; }
        [[nodiscard]] std::vector<std::byte> &bytes() noexcept { return data_; }

        /// Zero-copy view (only safe if the buffer really contains UTF-8/text).
        [[nodiscard]] std::string_view as_string_view() const noexcept
        {
            return std::string_view(reinterpret_cast<const char *>(data_.data()), data_.size());
        }

        /// Copies bytes into a std::string (only meaningful for text payloads).
        [[nodiscard]] std::string to_string() const
        {
            return std::string(as_string_view());
        }

        // --------------------------- Mutators ---------------------------
        void clear() noexcept { data_.clear(); }
        void resize(std::size_t size) { data_.resize(size); }

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
            const auto old = data_.size();
            data_.resize(old + text.size());
            if (!text.empty())
            {
                std::memcpy(data_.data() + old, text.data(), text.size());
            }
        }

        void append(std::span<const std::byte> bytes)
        {
            data_.insert(data_.end(), bytes.begin(), bytes.end());
        }

    private:
        std::vector<std::byte> data_{};
    };

} // namespace rix::io
