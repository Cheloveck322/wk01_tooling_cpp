# hexdump-lite

Мини-утилита на C++ для построчного дампа байтов: слева смещение (HEX), по центру байты (HEX), справа печатаемые ASCII. Проект — учебный трек по системному C++: инструменты → POSIX I/O → форматирование → тестирование/качество.

---

## Статус

- **День 1:** каркас проекта, CMake Presets (Debug/Release), бинарь печатает `--help`, санитайзеры в Debug.  
- **День 2:** определены интерфейсы и контракты:
  - `CliOptions` — контракт CLI (валидация значений, UX-сообщения),
  - `FileReader` — RAII над вводом (файл/`stdin`), POSIX `open/read/lseek/close`,
  - `HexFormatter` — чистое форматирование одной строки дампа.
  - Док: `docs/design_day2.md` (RAII/rule of five/zero, формат, политика ошибок) и тест-план.
- **День 3:** реализован `FileReader` (POSIX), добавлен каркас GoogleTest; «дымовые» проверки проходят.
- **Дальше (День 4+):** реализация `HexFormatter` + полноценные тесты, затем парсер CLI, статанализ и CI.

---

## Требования

- Linux/WSL2 или другая POSIX-система  
- CMake ≥ 3.20, Ninja, GCC/Clang (C++20)  
- Инструменты: `clang-tidy`, `cppcheck`, `valgrind` (по желанию)

Установка (Ubuntu/WSL2):
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build git clang lldb                     cppcheck clang-tidy valgrind
```

---

## Сборка

Используются CMake Presets.

```bash
# Конфигурация
cmake --preset debug
cmake --preset release

# Сборка
cmake --build --preset b-debug   -j
cmake --build --preset b-release -j
```

- **Debug**: включены `-fsanitize=address,undefined` и `-fno-omit-frame-pointer`.  
- **Release**: оптимизации, санитайзеры выключены.

Проверка наличия бинаря:
```bash
file build/debug/hexdump-lite
```

---

## Запуск (CLI)

> На момент Дней 1–3 доступен `--help`; реализация парсинга/форматирования подключается на следующих шагах. Ниже — **целевой контракт** CLI.

```
Usage: hexdump-lite [options] [file]

Options:
  -w, --width <8|16|32>   Кол-во байт на строку (по умолчанию 16)
  -n, --limit <N>         Ограничить суммарный вывод N байт (по умолчанию ∞)
  -o, --offset <N>        Начать с байта N (seek для файлов; для stdin — не поддерживается)
  -h, --help              Показать справку
```

Правила:
- Если `file` не указан → читаем из `stdin`.  
- `width` ∈ {8,16,32}.  
- `limit ≥ 0`, `offset ≥ 0` (целые без знака).  
- Коды возврата: `0` — успех/`--help`, `1` — ошибка аргументов, `2` — ошибки ввода-вывода.

---

## «Дымовые» проверки без тестов

Сборка Debug:
```bash
cmake --preset debug
cmake --build --preset b-debug -j
```

Базово:
```bash
./build/debug/hexdump-lite --help ; echo "exit=$?"
```

(Когда подключишь чтение/форматирование — сможешь сразу проверять:)
```bash
# STDIN
printf 'Hello\n' | ./build/debug/hexdump-lite -w 8

# Ровный и неровный файлы
dd if=/dev/zero    of=/tmp/zero_32.bin bs=1 count=32 status=none
dd if=/dev/urandom of=/tmp/rand_45.bin bs=1 count=45 status=none
./build/debug/hexdump-lite -w 16 /tmp/zero_32.bin
./build/debug/hexdump-lite -w 16 /tmp/rand_45.bin

# Offset + limit
./build/debug/hexdump-lite -w 8 -o 10 -n 16 /tmp/rand_45.bin

# Санитайзеры и утечки
ASAN_OPTIONS=detect_leaks=1 ./build/debug/hexdump-lite -n 64 /bin/ls
valgrind --quiet --leak-check=full ./build/debug/hexdump-lite -n 64 /bin/ls

# Быстрый strace (ожидаем open/read/lseek/close)
strace -e trace=open,read,lseek,close -o /tmp/trace.log   ./build/debug/hexdump-lite -n 16 /tmp/zero_32.bin
tail -n +1 /tmp/trace.log
```

---

## Спецификация формата строки (целевой)

Для каждой строки печатается:

```
OOOOOOOO  XX XX XX ... (width байт)  |ASCII...|
```

- `OOOOOOOO` — смещение в HEX (UPPERCASE) с ведущими нулями (минимум 8 разрядов).  
- HEX-часть: байты по 2 HEX-символа, разделены пробелом, всего `width` позиций; если вход короче → пустые позиции заполняются пробелами.  
- ASCII-часть: печатаемые символы диапазона `0x20..0x7E`; остальные → `.`.

Примеры:
```
00000000  48 65 6C 6C 6F 0A                   |Hello.|
00000010  00 00 00 00 00 00 00 00             |........|
```

---

## Архитектура модулей

- `CliOptions` — контракт CLI, валидация значений, человекочитаемые ошибки (без низкоуровневого I/O).  
- `FileReader` — RAII над источником (файл/`stdin`), POSIX `open/read/lseek/close`, move-only, без исключений в I/O методах.  
- `HexFormatter` — чистое форматирование: на вход смещение, окно байтов и `width`; на выход — готовая строка.

`main.cpp` — только «склейка»: парсинг/валидация → выбор источника → `seek` → цикл `read_chunk → format_line → print` → коды возврата.

---

## Политика ошибок

- **Уровень I/O (`FileReader`)**: методы чтения/seek **не бросают**; возвращают коды/флаги; подробность можно получить через `errno`/внутренний код.  
- **Уровень CLI/main**: преобразует ошибки в человекочитаемые сообщения и коды возврата.  
- **Коды выхода**: `0` — OK/`--help`; `1` — аргументы; `2` — I/O.

---

## Тестирование

GoogleTest подключается через `FetchContent`. Запуск:
```bash
ctest --preset t-debug --output-on-failure
```

План тестов (см. `docs/design_day2.md`):
- `HexFormatter`: `width=8/16/32`, короткая строка, непечатаемые, большие offset’ы — «golden» строки.  
- `FileReader`: несуществующий файл; `stdin` (через pipe); файл `N*width+k`; `seek` на середину/конец/за пределы.