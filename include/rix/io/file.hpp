#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <system_error>
#include <cstddef>
#include <cerrno>

namespace rix::io
{

    enum FileMode
    {
        Read,
        Write,
        Append,
        ReadWrite
    };

    enum class FileType
    {
        Text,
        Binary
    };

    inline std::ios_base::openmode to_std_openmode(FileMode mode, FileType type)
    {
        using std::ios_base;
        ios_base::openmode m{};

        switch (mode)
        {
        case FileMode::Read:
            m |= ios_base::in;
            break;
        case FileMode::Write:
            m |= ios_base::out | ios_base::trunc;
            break;
        case FileMode::Append:
            m |= ios_base::out | ios_base::app;
            break;
        case FileMode::ReadWrite:
            m |= ios_base::in | ios_base::out;
            break;
        }

        if (type == FileType::Binary)
        {
            m |= ios_base::binary;
        }

        return m;
    }

    class File
    {
    public:
        File() = default;

        File(const std::filesystem::path &path,
             FileMode mode,
             FileType type = FileType::Text)
            : path_(path), type_(type)
        {
            open_internal(mode, type_);
        }

        ~File()
        {
            close();
        }

        File(const File &) = delete;
        File &operator=(const File &) = delete;

        File(File &&other) noexcept
            : path_(std::move(other.path_)), stream_(std::move(other.stream_)), type_(other.type_)
        {
        }

        File &operator=(File &&other) noexcept
        {
            if (this != &other)
            {
                close();
                path_ = std::move(other.path_);
                stream_ = std::move(other.stream_);
                type_ = other.type_;
            }
            return *this;
        }

        [[nodiscard]] bool is_open() const noexcept
        {
            return stream_.is_open();
        }

        explicit operator bool() const noexcept
        {
            return is_open();
        }

        [[nodiscard]] const std::filesystem::path &path() const noexcept
        {
            return path_;
        }

        void close() noexcept
        {
            if (stream_.is_open())
            {
                stream_.close();
            }
        }

        [[nodiscard]] std::string read_all_text()
        {
            if (!is_open())
            {
                throw std::runtime_error("File not open for reading: " + path_.string());
            }

            std::string content;
            stream_.seekg(0, std::ios::end);
            const auto size = stream_.tellg();
            if (size > 0)
            {
                content.reserve(static_cast<std::size_t>(size));
            }
            stream_.seekg(0, std::ios::beg);

            content.assign(std::istreambuf_iterator<char>(stream_),
                           std::istreambuf_iterator<char>());

            return content;
        }

        [[nodiscard]] std::vector<std::byte> read_all_bytes()
        {
            if (!is_open())
            {
                throw std::runtime_error("File not open for reading: " + path_.string());
            }

            stream_.seekg(0, std::ios::end);
            auto end = stream_.tellg();
            stream_.seekg(0, std::ios::beg);
            auto begin = stream_.tellg();

            if (end < begin)
            {
                throw std::runtime_error("Invalid file size for: " + path_.string());
            }

            auto size = static_cast<std::size_t>(end - begin);
            std::vector<std::byte> buffer(size);

            stream_.read(reinterpret_cast<char *>(buffer.data()),
                         static_cast<std::streamsize>(size));

            return buffer;
        }

        void write(std::string_view text)
        {
            if (!is_open())
            {
                throw std::runtime_error("File not open for writing: " + path_.string());
            }
            stream_.write(text.data(), static_cast<std::streamsize>(text.size()));
            if (!stream_)
            {
                throw std::runtime_error("Failed to write to file: " + path_.string());
            }
        }

        void write(std::span<const std::byte> bytes)
        {
            if (!is_open())
            {
                throw std::runtime_error("File not open for writing: " + path_.string());
            }
            stream_.write(reinterpret_cast<const char *>(bytes.data()),
                          static_cast<std::streamsize>(bytes.size()));
            if (!stream_)
            {
                throw std::runtime_error("Failed to write to file: " + path_.string());
            }
        }

        void flush()
        {
            if (!is_open())
            {
                return;
            }
            stream_.flush();
        }

    private:
        std::filesystem::path path_{};
        std::fstream stream_{};
        FileType type_{FileType::Text};

        void open_internal(FileMode mode, FileType type)
        {
            auto flags = to_std_openmode(mode, type);
            stream_.open(path_, flags);

            if (!stream_.is_open())
            {
                throw std::system_error(
                    errno,
                    std::generic_category(),
                    "Failed to open file: " + path_.string());
            }
        }
    };

} // namespace rix::io
