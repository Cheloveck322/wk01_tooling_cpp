#ifndef HEXFMT_HPP
#define HEXFMT_HPP

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

std::string hex_format_line(std::uint64_t offset, std::span<const std::byte> data, std::size_t width);

#endif