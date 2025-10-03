#ifndef ARGS_HPP
#define ARGS_HPP

#include <cstddef>
#include <optional>
#include <string>

struct Options
{
    std::size_t width;
    std::size_t limit;
    std::size_t offset;
    std::optional<std::string> file;

};

Options parse_cli(int argc, char** argv);
void print_help(std::string_view prog, std::ostream& out);

#endif