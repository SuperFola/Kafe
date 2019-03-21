#include <kafe/parser.hpp>
#include <iostream>  // testing

using namespace kafe;

Parser::Parser(const std::string& code) :
    internal::Parser(code)
{}

Parser::~Parser()
{}

void Parser::parse()
{
    // parse until the end of the string
    while (!isEOF())
    {
        auto current = getCount();
        if (parseDeclaration())
            std::cout << "parsed declaration" << std::endl;
        else
        {
            std::cout << "error" << std::endl;
            auto old = getCount();
            back(getCount() - current + 1);
            std::cout
                << current << " "
                << old << " "
                << getCol() << " "
                << getRow() << " "
                << getCount()
                << std::endl;
            break;
        }
    }
}

bool Parser::parseDeclaration()
{
    /*
        Trying to parse those kind of expression:
        x : type
        x : type = value
    */

    // eat the trailing white space
    space();

    std::string varname = "";
    if (!name(&varname))
        return false;
    
    space();
    // : after varname and before type is mandatory
    if (!accept(internal::IsChar(':')))
        return false;
    space();

    std::string type = "";
    if (!name(&type))
        return false;
    
    space();
    // checking for value (optional)
    if (!accept(internal::IsChar('=')))
    {
        std::cout << "VAR(" << varname << "), "
            << "TYPE(" << type << ")" << std::endl;
        return true;
    }
    else
    {
        space();

        // testing, we'll need another parser for that
        std::string value = "";
        if (!name(&value))
            return false;
        
        std::cout << "VAR(" << varname << "), "
            << "TYPE(" << type << "), "
            << "VALUE(" << value << ")" << std::endl;
        return true;
    }
}