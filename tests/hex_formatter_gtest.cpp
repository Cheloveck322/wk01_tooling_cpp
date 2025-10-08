// tests/hex_formatter_tests.cpp
#include <gtest/gtest.h>
#include "../include/hex_formatter.hpp"

#include <vector>
#include <span>
#include <cstddef>
#include <string>

// Утилита: удобная сборка вектора байт из чисел 0..255
static std::vector<std::byte> bytes(std::initializer_list<unsigned> v) {
  std::vector<std::byte> out;
  out.reserve(v.size());
  for (auto x : v) out.push_back(static_cast<std::byte>(x & 0xFF));
  return out;
}

TEST(HexFormatter, Width8_PartialLine_HelloLF) {
  // "Hello\n"
  auto buf = bytes({0x48,0x65,0x6C,0x6C,0x6F,0x0A});
  // width = 8, offset = 0
  // Правила форматирования (зафиксированы тестом):
  // - offset: 8 HEX UPPERCASE с leading zeros, затем два пробела
  // - затем HEX-байты с пробелом после каждого
  // - недостающие позиции: три пробела на каждый "пустой" байт
  // - затем один пробел, ASCII-часть в формате |...|
  // - ASCII только для реально имеющихся байт (без добивки)
  std::string exp =
    "00000000  "        // offset + два пробела
    "48 65 6C 6C 6F 0A " // 6 байт + пробел после каждого
    "      "             // 2 недостающих байта => 2*3 пробела = 6
    " |Hello.|";         // ASCII: \n -> '.'

  auto got = hex_format_line(0u, std::span<const std::byte>(buf), /*width*/8);
  EXPECT_EQ(got, exp);
}

TEST(HexFormatter, Width16_FullLine_Mixed) {
  // 16 байт, смесь печатаемых и непечатаемых
  auto buf = bytes({
    0x00,0x01,0x02,0x03,0x41,0x42,0x7F,0x20, 0x30,0x31,0x32,0x33,0x61,0x62,0x63,0x0A
  });
  // offset = 0x30
  std::string exp =
    "00000030  "
    "00 01 02 03 41 42 7F 20 30 31 32 33 61 62 63 0A "
    " |....AB. 0123abc.|";

  auto got = hex_format_line(0x30u, std::span<const std::byte>(buf), /*width*/16);
  EXPECT_EQ(got, exp);
}

TEST(HexFormatter, NonPrintableMappedToDot) {
  auto buf = bytes({0x00,0x1F,0x20,0x21}); // .. ' ' '!'
  auto got = hex_format_line(0u, std::span<const std::byte>(buf), /*width*/8);
  // Проверяем только ASCII-часть для устойчивости
  auto pos = got.find('|');
  ASSERT_NE(pos, std::string::npos);
  auto pos2 = got.find('|', pos+1);
  ASSERT_NE(pos2, std::string::npos);
  auto ascii = got.substr(pos+1, pos2-pos-1);
  EXPECT_EQ(ascii, ".. !");
}

TEST(HexFormatter, LargeOffset_UppercaseHex) {
  auto buf = bytes({0xAA, 0xBB});
  auto got = hex_format_line(0xABCDEF10u, std::span<const std::byte>(buf), /*width*/8);
  // offset должен быть UPPERCASE HEX с ведущими нулями и длиной 8
  EXPECT_EQ(got.substr(0, 8), "ABCDEF10");
  EXPECT_EQ(got[8], ' ');
  EXPECT_EQ(got[9], ' ');
}
