#include "../include/cli_options.hpp"
#include <iostream>

Options parse_cli(int argc, char** argv)
{
    Options opt;
    for (int i = 1; i < argc; ++i)
    {
        std::string option{ argv[i] };
        if (option == "-h" || option == "--help")
        {
            print_help(argv[i], std::cout);
            std::exit(0);
        }
        else if (option == "-w" || option == "--width")
        {
            size_t new_width{ static_cast<size_t>(std::stoull(argv[++i])) };
            if (!(new_width == 8 || new_width == 16 || new_width == 32)) 
            {
                std::cerr << "Width must be 8, 16, or 32\n"; std::exit(1);
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
            std::cerr << "Unknown option: " << option << std::endl;
            print_help(argv[1], std::cout);
        }
        else 
        {
            opt._file = option;
        }
    }

    return opt;
}

void print_help(std::string_view prog, std::ostream& out)
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

void print_error_width(std::ostream& out)
{
    out << "Width must be 8, 16, or 32\n";
}

void print_error_size(std::string_view prog, std::ostream& out)
{
    out << "This command " << prog << " has to be >= 0\n";
}

int Options::validate() const
{
    
}   