#include "../include/cli_options.hpp"
#include <iostream>

Options Option::parse_args(int argc, char** argv)
{
    Options opt;
    for (int i = 1; i < argc; ++i)
    {
        std::string option{ argv[i] };
        if (option == "-h" || option == "--help")
        {
            opt._show_help = true;
            std::exit(0);
        }
        else if (option == "-w" || option == "--width")
        {
            size_t new_width{ static_cast<size_t>(std::stoull(argv[++i])) };
            if (!(new_width == 8 || new_width == 16 || new_width == 32)) 
            {
                opt._error = 1;
                std::exit(1);
            }
            else 
            {
                opt._width = new_width;
            }
        }
        else if (option == "-n" || option == "--limit")
        {
            opt._limit = static_cast<size_t>(std::stoull(argv[++i]));
        }
        else if (option == "-o" || option == "--offset")
        {
            opt._offset = static_cast<size_t>(std::stoull(argv[++i]));
        }
        else if (option.rfind("-", 0) != std::string::npos)
        {
            opt._error = 1;
            std::exit(1);
        }
        else 
        {
            opt._file = option;
        }
    }

    return opt;
}

void Option::print_help(std::string_view prog, std::ostream& out)
{
    out << 
        "Usage: " << prog << "\thexdump-lite [options] <_file>...\n"
        "Display _file contents in hexadcimal or ascii.\n"
        "Opitons:\n"
        "  -w, --width <8|16|32>   bytes per line (default 16)\n"
        "  -n, --limit <N>         limit total bytes\n"
        "  -o, --offset <N>        start offset\n"
        "  -h, --help              show this help\n";
}

Validate Options::validate() const
{
    Validate valid{ _error };

    return valid;
}   

std::string Validate::message() const noexcept
{
    switch (_type_error)
    {
    case 1:
        return "Invalid arguments error occurs. Pls try --help.";
    
    default:
        return "I/O file error occurs, pls try --help.\n";
    }
    return "";
}