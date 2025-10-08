#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <span>
#include <cstdint>
#include <cstdio>
#include <string_view>

class File_Reader
{
public:
    static File_Reader from_path(std::string_view path) noexcept;
    static File_Reader from_stdin() noexcept;

    File_Reader() noexcept = default;
    explicit File_Reader(std::string_view ch) noexcept;

    File_Reader(const File_Reader& file) = delete;
    File_Reader(File_Reader& file) = delete;
    File_Reader& operator=(const File_Reader& file) = delete;

    File_Reader(File_Reader&& file) noexcept;
    File_Reader& operator=(File_Reader&& file) noexcept;
    ~File_Reader() noexcept;

    [[nodiscard]] bool is_open() const noexcept { return _fd >= 0; };
    [[nodiscard]] int native_handle() const noexcept { return _fd; };
    int last_error() const noexcept { return _last_errno; };
    std::string_view last_op() const noexcept { return _last_op; };
    size_t read_chunk(std::span<std::byte> out) noexcept;
    bool seek(std::uint64_t offset) noexcept;

private:
    explicit File_Reader(int fd, bool own)
        : _fd{ fd }, _owns_fd{ own } {}
    
    void close_file_if_needed() noexcept;

    int _fd{ -1 };
    bool _owns_fd{ false };
    int _last_errno{ 0 };
    std::string_view _last_op{};
};

#endif