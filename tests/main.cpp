#include <iostream>
#include <kafe/kafe.hpp>

int main()
{
    std::cout << "testing Kafe" << std::endl;

    {
        kafe::Parser p;
        p.feed("x:type=test");
        p.parse();
    }

    {
        kafe::Parser p;
        p.feed("x : type=test");
        p.parse();
    }

    {
        kafe::Parser p;
        p.feed("x:type = test");
        p.parse();
    }

    {
        kafe::Parser p;
        p.feed("x:type");
        p.parse();
    }

    {
        kafe::Parser p;
        p.feed("x::type=test");
        p.parse();
    }

    return 0;
}