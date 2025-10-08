#include "../include/cli_options.hpp"
#include "../include/hex_formatter.hpp"
#include "../include/file_reader.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

// Simple program exit codes used by this project.
// Define them here so main.cpp can compile standalone.
constexpr int EXIT_OK   = 0;
constexpr int EXIT_ARGS = 1;
constexpr int EXIT_IO   = 3;

int main(int argc, char** argv) try {
  // 1) Разбор и валидация CLI
  Options opt = parse_cli(argc, argv);  

  // 2) Источник ввода: путь или stdin
  File_Reader in = opt._file ? File_Reader::from_path(*opt._file)
                           : File_Reader::from_stdin();

  // 3) Смещение (offset)
  if (opt._offset > 0) {
    if (!in.seek(opt._offset)) {
      // Для stdin или ошибки lseek
      std::cerr << "seek failed or not supported\n";
      // Решение: можно продолжить без seek (для stdin) или вернуть EXIT_IO — как задокументировано.
      return EXIT_IO;
    }
  }

  // 4) Основной цикл: читаем чанками, форматируем, печатаем
  std::size_t width     = opt._width;         // 8|16|32
  std::size_t remaining = opt._limit;         // SIZE_MAX = «без лимита»
  std::uint64_t offset  = opt._offset;        // текущее смещение для форматтера

  std::vector<std::byte> buffer(width);      // единый буфер под строку
  while (remaining > 0) {
    std::size_t to_read = std::min<std::size_t>(width, remaining);
    // ВАЖНО: читать в первые to_read байт buffer'а
    std::size_t got = in.read_chunk(std::span<std::byte>(buffer.data(), to_read));
    if (got == 0) break; // EOF или ошибка (см. контракт read_chunk)

    auto line = hex_format_line(offset,      /*смещение слева*/
                                          std::span<const std::byte>(buffer.data(), got),
                                          width);      /*заданная ширина строки*/
    std::cout << line << '\n';

    offset    += got;
    if (remaining != SIZE_MAX) remaining -= got;
  }

  // 5) Успешный выход
  return EXIT_OK;
}
catch (const std::exception& e) {
  // На всякий случай: превращаем «неожиданные» исключения в понятное сообщение
  std::cerr << "fatal: " << e.what() << "\n";
  return EXIT_ARGS; // или 1 по вашей политике
}
