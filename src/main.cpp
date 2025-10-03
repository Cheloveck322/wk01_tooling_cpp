#include "args.hpp"
#include "hexfmt.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) try 
{
    Options opt = parse_cli(argc, argv);

    std::istream* in = &std::cin;
    std::ifstream file;
    if (opt.file) 
    {
        file.open(*opt.file, std::ios::binary);
        if (!file) { std::cerr << "Failed to open: " << *opt.file << "\n"; return 2; }
        in = &file;
    }

    std::vector<std::byte> buf(opt.width);
    std::uint64_t offset = opt.offset;
    if (opt.offset && in->good()) {
        in->seekg(static_cast<std::streamoff>(opt.offset), std::ios::beg);
    }

    std::size_t printed = 0;
    while (in->good() && (printed < opt.limit)) 
    {
        in->read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(opt.width));
        std::streamsize got = in->gcount();
        if (got <= 0) break;

        std::span<const std::byte> span{buf.data(), static_cast<std::size_t>(got)};
        std::cout << format_line(offset, span, opt.width) << "\n";

        offset  += static_cast<std::size_t>(got);
        printed += static_cast<std::size_t>(got);
        if (printed >= opt.limit) break;
    }
    return 0;
}
catch (const std::exception& e) 
{
    std::cerr << "Error: " << e.what() << "\n"; return 1;
}
