#pragma once

#include <cstddef>      // std::byte, std::size_t
#include <cerrno>       // errno
#include <filesystem>   // std::filesystem::path
#include <fstream>      // std::fstream
#include <span>         // std::span
#include <string>       // std::string
#include <string_view>  // std::string_view
#include <system_error> // std::system_error
#include <stdexcept>    // std::runtime_error
#include <vector>       // std::vector
#include <iterator>     // std::istreambuf_iterator

namespace rix::io
{
    // Convention: enums publics => enum class (évite collisions)
    enum class FileMode
    {
        read,
        write, // truncate
        append,
        read_write
    };

    enum class FileType
    {
        text,
        binary
    };

    namespace detail
    {
        inline std::ios_base::openmode to_openmode(FileMode mode, FileType type)
        {
            using std::ios_base;

            ios_base::openmode m{};

            switch (mode)
            {
            case FileMode::read:
                m |= ios_base::in;
                break;
            case FileMode::write:
                m |= ios_base::out | ios_base::trunc;
                break;
            case FileMode::append:
                m |= ios_base::out | ios_base::app;
                break;
            case FileMode::read_write:
                m |= ios_base::in | ios_base::out;
                break;
            }

            if (type == FileType::binary)
            {
                m |= ios_base::binary;
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

    class File
    {
    public:
        File() = default;

        File(const std::filesystem::path &path,
             FileMode mode,
             FileType type = FileType::text)
            : path_(path), mode_(mode), type_(type)
        {
            open_internal();
        }

        ~File() { close(); }

        File(const File &) = delete;
        File &operator=(const File &) = delete;

        File(File &&other) noexcept
            : path_(std::move(other.path_)),
              stream_(std::move(other.stream_)),
              mode_(other.mode_),
              type_(other.type_)
        {
        }

        File &operator=(File &&other) noexcept
        {
            if (this != &other)
            {
                close();
                path_ = std::move(other.path_);
                stream_ = std::move(other.stream_);
                mode_ = other.mode_;
                type_ = other.type_;
            }
            return *this;
        }

        [[nodiscard]] bool is_open() const noexcept { return stream_.is_open(); }
        explicit operator bool() const noexcept { return is_open(); }

        [[nodiscard]] const std::filesystem::path &path() const noexcept { return path_; }
        [[nodiscard]] FileMode mode() const noexcept { return mode_; }
        [[nodiscard]] FileType type() const noexcept { return type_; }

        void close() noexcept
        {
            if (stream_.is_open())
            {
                stream_.close();
            }
        }

        // ---------------------------- Read ----------------------------
        [[nodiscard]] std::string read_all_text()
        {
            require_open();
            require_readable();

            // rewind
            stream_.clear();
            stream_.seekg(0, std::ios::beg);

            std::string content;
            content.assign(std::istreambuf_iterator<char>(stream_),
                           std::istreambuf_iterator<char>());

            if (!stream_.eof() && stream_.fail())
            {
                throw std::runtime_error("Failed to read text from file: " + path_.string());
            }

            return content;
        }

        [[nodiscard]] std::vector<std::byte> read_all_bytes()
        {
            require_open();
            require_readable();

            stream_.clear();
            stream_.seekg(0, std::ios::end);
            const auto end = stream_.tellg();
            stream_.seekg(0, std::ios::beg);
            const auto begin = stream_.tellg();

            if (end < begin)
            {
                throw std::runtime_error("Invalid file size for: " + path_.string());
            }

            const auto size = static_cast<std::size_t>(end - begin);
            std::vector<std::byte> buffer(size);

            if (size > 0)
            {
                stream_.read(reinterpret_cast<char *>(buffer.data()),
                             static_cast<std::streamsize>(size));

                if (static_cast<std::size_t>(stream_.gcount()) != size || stream_.fail())
                {
                    throw std::runtime_error("Failed to read bytes from file: " + path_.string());
                }
            }

            return buffer;
        }

        // ---------------------------- Write ---------------------------
        void write(std::string_view text)
        {
            require_open();
            require_writable();

            stream_.write(text.data(), static_cast<std::streamsize>(text.size()));
            if (!stream_)
            {
                throw std::runtime_error("Failed to write to file: " + path_.string());
            }
        }

        void write(std::span<const std::byte> bytes)
        {
            require_open();
            require_writable();

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
                return;
            stream_.flush();
        }

    private:
        std::filesystem::path path_{};
        std::fstream stream_{};
        FileMode mode_{FileMode::read};
        FileType type_{FileType::text};

        void open_internal()
        {
            const auto flags = detail::to_openmode(mode_, type_);
            stream_.open(path_, flags);

            if (!stream_.is_open())
            {
                throw std::system_error(
                    errno,
                    std::generic_category(),
                    "Failed to open file: " + path_.string());
            }
        }

        void require_open() const
        {
            if (!is_open())
            {
                throw std::runtime_error("File not open: " + path_.string());
            }
        }

        void require_readable() const
        {
            if (!detail::mode_can_read(mode_))
            {
                throw std::runtime_error("File not open for reading: " + path_.string());
            }
        }

        void require_writable() const
        {
            if (!detail::mode_can_write(mode_))
            {
                throw std::runtime_error("File not open for writing: " + path_.string());
            }
        }
    };

} // namespace rix::io
