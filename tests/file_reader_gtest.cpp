// tests/file_reader_tests.cpp
#include <gtest/gtest.h>
#include "../include/file_reader.hpp"

#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <cstdio>

#include <unistd.h>   // pipe, write, close (POSIX)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace {

std::filesystem::path make_temp_file(const std::string& content) {
  auto p = std::filesystem::temp_directory_path() / "hexdump_test_tmp.bin";
  std::ofstream ofs(p, std::ios::binary);
  ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
  ofs.close();
  return p;
}

std::string bytes_to_string(std::span<const std::byte> s) {
  std::string out;
  out.reserve(s.size());
  for (auto b : s) out.push_back(static_cast<char>(std::to_integer<unsigned>(b)));
  return out;
}

} // namespace

TEST(File_Reader, OpenNonexistentFile) {
  File_Reader fr("this_file_does_not_exist.really");
  EXPECT_FALSE(fr.is_open());
  EXPECT_NE(fr.last_error(), 0); // errno-подобная ошибка
  // read_chunk в неоткрытом состоянии должен возвращать 0 и не падать
  std::array<std::byte, 8> buf{};
  EXPECT_EQ(fr.read_chunk(buf), 0u);
}

TEST(File_Reader, ReadFromFileInChunks) {
  auto p = make_temp_file(std::string(25, 'A')); // 25 байт
  File_Reader fr(p.string());
  ASSERT_TRUE(fr.is_open()) << "last_error=" << fr.last_error();

  std::array<std::byte, 8> buf{};
  std::size_t n1 = fr.read_chunk(buf);
  std::size_t n2 = fr.read_chunk(buf);
  std::size_t n3 = fr.read_chunk(buf);
  std::size_t n4 = fr.read_chunk(buf);
  std::size_t n5 = fr.read_chunk(buf);

  EXPECT_EQ(n1, 8u);
  EXPECT_EQ(n2, 8u);
  EXPECT_EQ(n3, 8u);
  EXPECT_EQ(n4, 1u);
  EXPECT_EQ(n5, 0u); // EOF
}

TEST(File_Reader, SeekAndReadTail) {
  auto p = make_temp_file("HelloWorld"); // 10 байт
  File_Reader fr(p.string());
  ASSERT_TRUE(fr.is_open());

  EXPECT_TRUE(fr.seek(5)); // на "World"
  std::array<std::byte, 8> buf{};
  std::size_t n = fr.read_chunk(buf);
  ASSERT_EQ(n, 5u);

  auto s = bytes_to_string(std::span<const std::byte>(buf.data(), n));
  EXPECT_EQ(s, "World");
}