#ifndef ARGS_HPP
#define ARGS_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <unordered_set>
#include <string_view>

const std::unordered_set<std::string_view> all_options{ "-h", "--help", "-w", "--width", "-n", "--limit", "-o", "--offset" };

class Validate
{
public: 
    Validate() = default;
    explicit Validate(int type_error) noexcept: _type_error{ type_error } {}

    bool has_error() const noexcept { return _type_error > 0; }
    std::string message() const noexcept;

private:
    int _type_error{}; // 
};

struct Options
{
    std::size_t _width{ 16 };
    std::size_t _limit{ 1000 };
    std::size_t _offset{ 0 };
    std::optional<std::string> _file;
    int _error;
    bool _show_help{ false };
    
    Validate validate() const; 
   
};

namespace Option
{
    Options parse_args(int argc, char** argv);
    void print_help(std::string_view prog, std::ostream& out);
}


#endif