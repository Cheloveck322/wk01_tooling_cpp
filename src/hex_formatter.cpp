#include "../include/hex_formatter.hpp"
#include <ios>
#include <iostream>
#include <iomanip>
#include <fstream>

std::string hex_format_line(std::uint64_t offset, std::span<const std::byte> data, std::size_t width)
{
    std::ofstream output_file{ "file.txt" };
    output_file.seekp(offset);
    for (const auto& d : data)
    {
        output_file << std::hex << std::setfill('0') << std::setw(width) << static_cast<int>(d) << " ";
    }
    output_file.close();

    std::ifstream input_file{ "file.txt" };
    std::string hex_string{};
    std::string hex_number{};
    
    while (input_file >> hex_number)
    {
        hex_string.append(hex_number);
    }

    std::cout << hex_string << '\n';

    return hex_string;
}