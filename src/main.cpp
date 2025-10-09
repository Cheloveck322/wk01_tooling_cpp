// src/main.cpp
#include <cstddef>
#include <cstdint>
#include <vector>
#include <span>
#include <iostream>
#include <cstring>
#include "../include/cli_options.hpp"    // Options::from_argv(), validate(), fields: show_help, width, limit, offset, file
#include "../include/file_reader.hpp"    // File_Reader::{from_path, from_stdin}, is_open(), seek(), read_chunk(), last_error()
#include "../include/hex_formatter.hpp"  // HexFormatter::format_line(offset, span<const std::byte>, width)

#if defined(__unix__) || defined(__APPLE__)
  #include <csignal>
#endif

namespace {

enum ExitCode : int {
  EXIT_OK   = 0,  // успех или --help
  EXIT_ARGS = 1,  // проблемы с аргументами/валидацией
  EXIT_IO   = 2   // проблемы ввода/вывода
};

// При необходимости преобразуй last_error() в человекочитаемое сообщение.
// Если last_error() возвращает errno — используем strerror; иначе выведем код как есть.
std::string to_error_message(int err) {
#if defined(__unix__) || defined(__APPLE__)
  if (err != 0) {
    return std::string(std::strerror(err));
  }
#endif
  return (err == 0) ? std::string("no error") : ("error code: " + std::to_string(err));
}

} // namespace

int main(int argc, char** argv) try {
  // ускоряем iostream над STDIO
  std::ios_base::sync_with_stdio(false);

#if defined(__unix__) || defined(__APPLE__)
  // не умирать молча, если потребитель пайпа закрыт (например: | head -n1)
  std::signal(SIGPIPE, SIG_IGN);
#endif

  // 1) Разобрать аргументы
  Options opt = Option::parse_args(argc, argv);  // подстрой под свой API, если он иной
  if (opt._show_help) 
  {
    Option::print_help(argv[1], std::cerr);
    return EXIT_OK;
  }
  if (auto v = opt.validate(); v.has_error()) {
    std::cerr << v.message() << '\n';
    return EXIT_ARGS;
  }

  // 2) Открыть источник: файл или stdin
  File_Reader in = opt._file ? File_Reader::from_path(*opt._file)
                           : File_Reader::from_stdin();

  if (!in.is_open()) {
    const int err = in.last_error();
    std::cerr << "open failed";
    if (opt._file) std::cerr << " for '" << *opt._file << "'";
    if (err) std::cerr << ": " << to_error_message(err);
    std::cerr << '\n';
    return EXIT_IO;
  }

  // 3) Применить offset (если > 0)
  if (opt._offset > 0) {
    if (!in.seek(static_cast<std::uint64_t>(opt._offset))) {
      // По нашей политике: для stdin seek не поддерживается → считаем это ошибкой и выходим,
      // либо (если так решено в дизайне) мягко продолжаем с начала (закрепи поведение в документации).
      std::cerr << "seek failed or not supported (offset=" << opt._offset << ")\n";
      return EXIT_IO;
    }
  }

  // 4) Основной цикл: читаем чанки и печатаем строки
  const std::size_t width = opt._width;                   // 8 | 16 | 32
  std::size_t remaining   = opt._limit;                   // SIZE_MAX = безлимит
  std::uint64_t cur_off   = static_cast<std::uint64_t>(opt._offset);

  std::vector<std::byte> buffer(width);

  while (remaining > 0) {
    const std::size_t to_read =
      (remaining == static_cast<std::size_t>(-1)) ? width
                                                  : std::min(width, remaining);

    // NB: читаем ровно в to_read первых байт буфера
    const std::size_t got = in.read_chunk(std::span<std::byte>(buffer.data(), to_read));

    if (got == 0) {
      // EOF или ошибка — read_chunk должен различать их через last_error(), но main
      // просто завершает вывод (поведение зафиксировано в дизайне).
      break;
    }

    const auto line =
      hex_format_line(cur_off,
                                std::span<const std::byte>(buffer.data(), got),
                                width);
    std::cout << line << '\n';

    cur_off += static_cast<std::uint64_t>(got);
    if (remaining != static_cast<std::size_t>(-1)) {
      remaining -= got;
    }
  }

  return EXIT_OK;
}
catch (const std::exception& e) {
  // Неожиданная ошибка на верхнем уровне: сообщаем и маппим в код аргументов
  std::cerr << "fatal: " << e.what() << '\n';
  return EXIT_ARGS;
}
catch (...) {
  std::cerr << "fatal: unknown error\n";
  return EXIT_ARGS;
}
