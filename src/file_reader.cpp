#include "../include/file_reader.hpp"
#include <cerrno>
#include <fcntl.h>
#include <string>
#include <unistd.h>

File_Reader File_Reader::from_path(std::string_view path) noexcept
{
    int fd = ::open(std::string(path).c_str() , O_RDONLY | O_CLOEXEC);
    if (fd < 0)
    {
        File_Reader fd;
        fd._last_errno = errno;
        fd._last_op = "open";
        return fd;
    }
    return File_Reader{ fd, true };
}

File_Reader::File_Reader(std::string_view ch) noexcept:
    File_Reader(File_Reader::from_path(ch))
{
}

File_Reader File_Reader::from_stdin() noexcept
{
    return File_Reader{ STDIN_FILENO, false };
}

File_Reader::File_Reader(File_Reader&& other) noexcept:
    _fd{ other._fd }, _owns_fd{ other._owns_fd }, _last_errno{ other._last_errno }, _last_op{ other._last_op }
{
    other._fd = -1;
    other._owns_fd = false;
    other._last_errno = 0;
    other._last_op = {};
}

File_Reader& File_Reader::operator=(File_Reader&& other) noexcept
{
    if (this != &other)
    {
        close_file_if_needed();
        _fd = other._fd;
        _owns_fd = other._fd;
        _last_errno = other._last_errno;
        _last_op = other._last_op;
        other._fd = -1;
        other._owns_fd = false;
        other._last_errno = 0;
        other._last_op = {};
    }
    return *this;
}

void File_Reader::close_file_if_needed() noexcept
{
    if (_fd >= 0 && _owns_fd)
    {
        if (::close(_fd) != 0)
        {
            _last_errno = errno;
            _last_op = "close";
        }
    }
    _fd = -1;
    _owns_fd = false;
}

File_Reader::~File_Reader() noexcept
{
    close_file_if_needed();
}

size_t File_Reader::read_chunk(std::span<std::byte> out) noexcept
{
    _last_errno = 0;
    _last_op = {};
    if (_fd < 0 || out.empty())
    {
        return 0;
    }

    const std::size_t to_read{ out.size() };
    std::size_t total{ 0 };

    for (;;)
    {
        ssize_t n{ ::read(_fd, out.data() + total, to_read - total) };
        if (n > 0)
        {
            total += static_cast<std::size_t>(n);
            break;
        }
        if (n == 0)
        {
            break;
        }
        if (errno == EINTR)
        {
            continue;
        } 

        _last_errno = errno;
        _last_op = "read";

        return 0;
    }

    return total;
}

bool File_Reader::seek(std::uint64_t offset) noexcept
{
    _last_errno = 0;
    _last_op = {};

    if (_fd < 0)
    {
        return false;
    }

    off_t res{ ::lseek(_fd, static_cast<off_t>(offset), SEEK_SET) };
    if (res == static_cast<off_t>(-1))
    {
        _last_errno = errno;
        _last_op = "seek";
        return false;
    }
    return true;
}