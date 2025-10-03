#include "../include/hexfmt.hpp"
#include <iomanip>
#include <sstream>

std::string format_line(std::uint64_t off, std::span<const std::byte> data, std::size_t width) 
{
    std::ostringstream out;
    out << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << off << "  ";

    // hex-часть
    for (std::size_t i = 0; i < width; ++i) {
        if (i < data.size()) {
        unsigned v = std::to_integer<unsigned>(data[i]);
        out << std::setw(2) << std::setfill('0') << v << ' ';
        } else {
        out << "   ";
        }
    }
    out << " |";
    // ascii-часть
    for (std::size_t i = 0; i < data.size(); ++i) {
        unsigned v = std::to_integer<unsigned>(data[i]);
        char c = (v >= 0x20 && v <= 0x7E) ? static_cast<char>(v) : '.';
        out << c;
    }
    out << "|";
    return out.str();
}
