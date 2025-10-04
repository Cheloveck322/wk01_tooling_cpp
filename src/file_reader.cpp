#include "../include/file_reader.hpp"

char* File_Reader::get_file_name() const
{
    return _file_name;
}

File_Reader::File_Reader(File_Reader&& other)
{
    _input = std::move(other._input);
    _file_name = std::move(other._file_name);

    other._input.close();
    other._file_name = nullptr;
}

File_Reader& File_Reader::operator=(File_Reader&& other) noexcept
{
    if (this != &other)
    {
        _input = std::move(other._input);
        _file_name = std::move(other._file_name);

        other._input.close();
        other._file_name = nullptr;
    }
    return *this;
}

bool File_Reader::is_open() const noexcept
{
    return _input.is_open();
}

size_t File_Reader::read_chunk(std::span<std::byte> out) noexcept
{
    return 0;
}

bool File_Reader::seek(std::uint64_t offset) noexcept
{
    return false;
}

int File_Reader::native_handle() const noexcept
{
    return 1;   
}