#include "main.hpp"

#include <iostream>
#include <kafe/kafe.hpp>
#include <sstream>
#include <filesystem>
#include <string>

int main()
{
    std::cout << "Kafe tests" << std::endl;

    // getting all the tests in the directory kafe/
    std::vector<std::string> v;
    std::filesystem::path p("./kafe/");
    std::filesystem::directory_iterator start(p);
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

    // testing each file :
    //    comparing generated AST and output
    for (auto file: files)
    {
        // parsing
        kafe::Parser p(readFile(file));
        p.parse();

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
            std::cout << "Failed test " << i << std::endl;
        }
        
        ++i;
    }

    std::cout << std::endl << std::endl
        << "Tests passed: " << passed << "/" << i << std::endl
        << "Tests failed: " << failed << "/" << i << std::endl;

    return 0;
}