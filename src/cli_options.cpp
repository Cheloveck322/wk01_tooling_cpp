#include "../include/cli_options.hpp"
#include <iostream>

Options parse_cli(int argc, char** argv)
{
    Options opt;
    for (int i = 1; i < argc; ++i)
    {
        std::string option{ argv[i] };
        if (i < argc && all_options.find(argv[i+1]) == all_options.end())
        {
            opt.file = option;
        }
        if (option == "-h" || option == "--help")
        {
            print_help(argv[i], std::cout);
            std::exit(0);
        }

        size_t next_argument{ static_cast<size_t>(std::stoull(argv[++i])) };
        if (next_argument < 0)
        {
            print_error_size(argv[i], std::cerr);
            std::exit(1);
        }
        if (option == "-w" || option == "--width")
        {
            if (!(next_argument == 8 || next_argument == 16 || next_argument == 32)) 
            {
                print_error_width(std::cerr);
                std::exit(1);
            }
            opt.width = next_argument;
        }
        else if (option == "-n" || option == "--limit")
        {
            opt.limit = next_argument;
        }
        else if (option == "-o" || option == "--offset")
        {
            opt.offset = next_argument;
        }
        else if (option.rfind("-", 0) != std::string::npos)
        {
            std::cerr << "Unknown option: " << option << std::endl;
            print_help(argv[i], std::cout);
        }
    }

    return opt;
}

void print_help(std::string_view prog, std::ostream& out)
{
    out << 
        "Usage: " << prog << "\thexdump-lite [options] <file>...\n"
        "Display file contents in hexadcimal or ascii.\n"
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