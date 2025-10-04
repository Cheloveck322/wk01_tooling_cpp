#ifndef FILE_READER_HPP
#define FILE_READER_HPP

#include <fstream>
#include <span>
#include <cstdint>

class File_Reader
{
public:
    explicit File_Reader(char* name)
        : _file_name{ name }, _input{ name }
    {
    }

    File_Reader(const File_Reader& file) = delete;
    File_Reader(File_Reader& file) = delete;
    File_Reader& operator=(const File_Reader& file) = delete;

    File_Reader(File_Reader&& file);
    File_Reader& operator=(File_Reader&& file) noexcept;
    bool is_open() const noexcept;
    size_t read_chunk(std::span<std::byte> out) noexcept;
    bool seek(std::uint64_t offset) noexcept;
    int native_handle() const noexcept;

    char* get_file_name() const;
private:
    char* _file_name{ nullptr };
    std::ifstream _input;
};

#endif