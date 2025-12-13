#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <span>
#include <cstddef>

namespace rix::io
{

    class Buffer
    {
    public:
        Buffer() = default;

        explicit Buffer(std::size_t size)
            : data_(size) {}

        explicit Buffer(const std::vector<std::byte> &bytes)
            : data_(bytes) {}

        explicit Buffer(std::vector<std::byte> &&bytes) noexcept
            : data_(std::move(bytes)) {}

        /// Construct from text (UTF-8 assumed) – stored as bytes.
        explicit Buffer(std::string_view text)
        {
            data_.resize(text.size());
            std::memcpy(data_.data(), text.data(), text.size());
        }

        [[nodiscard]] std::size_t size() const noexcept
        {
            return data_.size();
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return data_.empty();
        }

        [[nodiscard]] std::byte *data() noexcept
        {
            return data_.data();
        }

        [[nodiscard]] const std::byte *data() const noexcept
        {
            return data_.data();
        }

        [[nodiscard]] std::span<std::byte> span() noexcept
        {
            return {data_.data(), data_.size()};
        }

        [[nodiscard]] std::span<const std::byte> span() const noexcept
        {
            return {data_.data(), data_.size()};
        }

        [[nodiscard]] const std::vector<std::byte> &bytes() const noexcept
        {
            return data_;
        }

        [[nodiscard]] std::vector<std::byte> &bytes() noexcept
        {
            return data_;
        }

        [[nodiscard]] std::string to_string() const
        {
            return std::string(
                reinterpret_cast<const char *>(data_.data()),
                data_.size());
        }

        void clear() noexcept
        {
            data_.clear();
        }

        void resize(std::size_t size)
        {
            data_.resize(size);
        }

    private:
        std::vector<std::byte> data_;
    };

} // namespace rix::io
