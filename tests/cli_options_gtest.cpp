// tests/cli_options_tests.cpp
#include <gtest/gtest.h>
#include "../include/cli_options.hpp"
#include <limits>
#include <optional>

TEST(Options, DefaultsAreValid) {
  Options opt; // _width=16, _limit=SIZE_MAX, _offset=0, file=nullopt
  EXPECT_TRUE(opt.validate()) << opt._error;
  EXPECT_TRUE(opt._error.empty());
  EXPECT_EQ(opt._width, 16u);
  EXPECT_EQ(opt._offset, 0u);
}

TEST(Options, _WidthMustBeOneOf_8_16_32) {
  Options opt;
  opt._width = 7; // недопустимо
  EXPECT_FALSE(opt.validate());
  EXPECT_FALSE(opt._error.empty());
  // верни _width в валидный
  opt._width = 32;
  opt._error.clear();
  EXPECT_TRUE(opt.validate()) << opt._error;
}

TEST(Options, _LimitAnd_OffsetNonNegative) {
  Options opt;
  // _offset/_limit — size_t, отрицательных нет по типу, но можно проверить overflow/ограничения
  opt._offset = 0;
  opt._limit = std::numeric_limits<std::size_t>::max();
  EXPECT_TRUE(opt.validate()) << opt._error;
}

TEST(Options, SingleOptionalFileIsOk) {
  Options opt;
  EXPECT_FALSE(opt._file.has_value());
  EXPECT_TRUE(opt.validate()) << opt._error;

  opt._file = std::string("/tmp/x.bin");
  EXPECT_TRUE(opt.validate()) << opt._error;
}

TEST(Options, _ErrorMessageIsHumanReadable) {
  Options opt;
  opt._width = 5;
  EXPECT_FALSE(opt.validate());
  // Сообщение об ошибке желательно содержать ключевую подсказку:
  EXPECT_TRUE(opt._error.find("_width") != std::string::npos);
}
