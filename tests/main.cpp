#include <iostream>
#include <kafe/kafe.hpp>

int main()
{
    std::cout << "testing Kafe" << std::endl;

    {
        kafe::Parser p("x:type=\"test\"\nx : int = 42");
        p.parse();
        p.ASTtoString(std::cout);
        std::cout << "\n";
    }

    {
        kafe::Parser p("x : type=-1");
        p.parse();
        p.ASTtoString(std::cout);
        std::cout << "\n";
    }

    {
        kafe::Parser p("x:type = 1.2");
        p.parse();
        p.ASTtoString(std::cout);
        std::cout << "\n";
    }

    {
        kafe::Parser p("x:type");
        p.parse();
        p.ASTtoString(std::cout);
        std::cout << "\n";
    }

    {
        kafe::Parser p("x::type=true");
        p.parse();
        p.ASTtoString(std::cout);
        std::cout << "\n";
    }

    return 0;
}