// to print stuff and parse use Kafe
#include <iostream>
#include <kafe/kafe.hpp>

// to read files
#include <string>
#include <fstream>
#include <streambuf>

// for the tests (ASTtoString)
#include <sstream>

std::string readFile(const std::string& name)
{
    std::ifstream f(name.c_str());
    // admitting the file exists
    return std::string(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>()
    );
}

bool deepCompareString(const std::string& a, const std::string& b)
{
    std::string line = "";
    std::size_t index = 0;
    std::size_t row = 1;

    for (std::size_t i=0; i < a.size(); ++i)
    {
        if (i >= b.size())
        {
            std::cout << "b is too short" << std::endl;
            return false;
        }

        if (b[i] == '\n')
        {
            line = "";
            index = 0;
            ++row;
        }
        else
        {
            line += b[i];
            ++index;
        }
        
        if (a[i] != b[i])
        {
            std::cout << line << " (line: " << row << ")\n";
            if (index > 0)
            {
                for (std::size_t j=0; j < index - 1; ++j)
                    std::cout << "~";
            }
            std::cout << "^" << std::endl;
            return false;
        }
    }
    return true;
}

int main()
{
    std::cout << "testing Kafe" << std::endl;

    {
        // parsing
        kafe::Parser p(readFile("kafe/def.kafe"));
        p.parse();

        // getting AST
        std::ostringstream os;
        p.ASTtoString(os);

        // comparing with what we need to have
        auto content = readFile("kafe/def.kafe.expected");
        bool passed = deepCompareString(os.str(), content);
        if (passed)
            std::cout << "Passed" << std::endl;
        else
            std::cout << "Failed" << std::endl;
    }

    return 0;
}