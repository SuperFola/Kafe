#include "main.hpp"

#include <iostream>
#include <kafe/kafe.hpp>
#include <sstream>
#include <filesystem>
#include <string>


#include <cstdio>
#include <ctime>

void handleParseErrors(kafe::Parser& p)
{
    try
    {
        p.parse();
    }
    catch (const kafe::internal::ParseError& e)
    {
        std::cout << "ParseError: " << e.what() << " " << e.exp << std::endl;
        std::cout << "At " << ((char) e.sym) << " @ " << e.row << ":" << e.col << std::endl;
    }
}

int main()
{
    std::cout << "Kafe tests" << "\n"
              << "==========" << "\n" << std::endl;

    // getting all the tests in the directory kafe/
    std::vector<std::string> v;
    std::filesystem::path path("./kafe/");
    std::filesystem::directory_iterator start(path);
    std::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(v), path_leaf_string());

    // only keep the .kafe
    std::vector<std::string> files;
    for (auto f: v)
    {
        if (f.size() > 8 && f.substr(f.size() - 4) == "kafe")
            files.push_back(f);
    }

    std::size_t i = 0;
    std::size_t passed = 0;
    std::size_t failed = 0;


    /*std::string code = "";
    for (int i=0; i < 1000000; ++i)
        code += "x: int = 10\n";
    std::size_t chars = code.size();
    kafe::Parser p(code);

    double duration = 0.0;
    std::clock_t starttime = std::clock();
    p.parse();
    duration = (std::clock() - starttime) / (double) CLOCKS_PER_SEC;
    std::cout << duration << std::endl;

    return 0;*/

    // testing each file :
    //    comparing generated AST and output
    for (auto file: files)
    {
        // parsing
        kafe::Parser p(readFile(file));
        handleParseErrors(p);

        // getting AST
        std::ostringstream os;
        p.ASTtoString(os);

        // comparing with what we need to have
        auto content = readFile(file + ".expected");
        
        if (deepCompareString(os.str(), content))
            ++passed;
        else
        {
            ++failed;
            std::cout << "Test '" << file << "' (" << i << ") failed" << std::endl;
            std::cout << os.str() << std::endl;
            std::cout << "===========================" << std::endl;
            std::cout << content << std::endl;
        }
        
        ++i;
    }

    std::cout << std::endl << std::endl
        << "Tests passed: " << passed << "/" << i << std::endl
        << "Tests failed: " << failed << "/" << i << std::endl;

    return 0;
}