#include "../include/hex_formatter.hpp"

#include <cassert>

namespace 
{
constexpr char HEX[16] = 
{
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

inline bool is_printable(unsigned v) noexcept 
{
  return v >= 0x20u && v <= 0x7Eu;
}
} // namespace

std::string hex_format_line(std::uint64_t offset,
                        std::span<const std::byte> data,
                        std::size_t width) 
{
  // Простая защита контракта (можно заменить на precondition валидации выше по стеку)
  assert((width == 8u || width == 16u || width == 32u) && "width must be 8/16/32");
  assert(data.size() <= width && "data.size() must be <= width");

  const std::size_t data_len = data.size();

  // Подсказка: длина строки = 14 + width*3 + data_len
  const std::size_t L_total = 14u + width * 3u + data_len;
  std::string out;
  out.reserve(L_total);

  // 1) offset: 8 HEX, старшие цифры первыми (big-endian по 4 бита)
  for (int nib = 7; nib >= 0; --nib) {
    const unsigned v = static_cast<unsigned>((offset >> (nib * 4)) & 0xFULL);
    out.push_back(HEX[v]);
  }

  // 2) два пробела после offset
  out.push_back(' ');
  out.push_back(' ');

  // 3) HEX-часть: фактические байты
  for (std::size_t i = 0; i < data_len; ++i) {
    const unsigned v = static_cast<unsigned>(std::to_integer<unsigned char>(data[i]));
    out.push_back(HEX[(v >> 4) & 0xF]);
    out.push_back(HEX[v & 0xF]);
    out.push_back(' ');
  }
  //    добивка до width "   " (три пробела на «пустой» байт)
  for (std::size_t i = data_len; i < width; ++i) {
    out.push_back(' ');
    out.push_back(' ');
    out.push_back(' ');
  }

  // 4) пробел и открывающая '|' перед ASCII-панелью
  out.push_back(' ');
  out.push_back('|');

  // 5) ASCII-панель (только реальные байты)
  for (std::size_t i = 0; i < data_len; ++i) {
    const unsigned v = static_cast<unsigned>(std::to_integer<unsigned char>(data[i]));
    out.push_back(is_printable(v) ? static_cast<char>(v) : '.');
  }

  // 6) закрывающая '|'
  out.push_back('|');

  return out;
}
