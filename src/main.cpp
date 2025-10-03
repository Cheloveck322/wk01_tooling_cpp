#include "../include/args.hpp"
#include "../include/hexfmt.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) try 
{
    Options opt = parse_cli(argc, argv);

    return 0;
}
catch (const std::exception& e) 
{
    std::cerr << "Error: " << e.what() << "\n"; return 1;
}
