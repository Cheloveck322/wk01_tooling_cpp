#include "../include/cli_options.hpp"
#include "../include/hex_formatter.hpp"
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
