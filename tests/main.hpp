#ifndef main_hpp
#define main_hpp

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <filesystem>
#include <algorithm>

struct path_leaf_string
{
    std::string operator()(const std::filesystem::directory_entry& entry) const
    {
        return entry.path().string();
    }
};

inline std::string readFile(const std::string& name)
{
    std::ifstream f(name.c_str());
    // admitting the file exists
    return std::string(
        (std::istreambuf_iterator<char>(f)),
        std::istreambuf_iterator<char>()
    );
}

inline bool deepCompareString(const std::string& a, const std::string& b)
{
    std::string line = "";
    std::string line2 = "";
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

        if (a[i] == '\n')
            line2 = "";
        else
            line2 += a[i];
        
        if (a[i] != b[i])
        {
            std::cout << "\nDeepCompareString failed" << std::endl;
            std::cout << line << " (line: " << row << ")\n";
            if (index > 0)
            {
                for (std::size_t j=0; j < index - 1; ++j)
                    std::cout << "~";
            }
            std::cout << "^" << std::endl;
            std::cout << "Expected: " << line2 << "\n" << std::endl;
            return false;
        }
    }
    return true;
}

#endif