#ifndef ARGS_HPP
#define ARGS_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_set>
#include <string_view>

const std::unordered_set<std::string_view> all_options{ "-h", "--help", "-w", "--width", "-n", "--limit", "-o", "--offset" };

struct Options
{
    std::size_t _width{ 16 };
    std::size_t _limit{ 0 };
    std::size_t _offset{ 0 };
    std::optional<std::string> _file;

    // validate code: 0 - --help/okay, 1 - wrong arguments/not implemented, 2 - I/O error
    int is_validate() const; 
};

Options parse_cli(int argc, char** argv);
void print_help(std::string_view prog, std::ostream& out);
void print_error_width(std::ostream& out);
void print_error_size(std::string_view prog, std::ostream& out);

#endif