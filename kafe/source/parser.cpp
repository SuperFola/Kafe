#include <kafe/parser.hpp>
#include <iostream>  // testing

using namespace kafe;
using namespace kafe::internal;

Parser::Parser(const std::string& code) :
    internal::Parser(code)
    , m_node(nullptr)
{}

Parser::~Parser()
{}

void Parser::parse()
{
    // parse until the end of the string
    while (!isEOF())
    {
        // save current position in buffer to be able to go back if needed
        auto current = getCount();

        // TEST
        if (!parseDeclaration())
            break;  // back(getCount() - current + 1);
    }

    std::cout << m_program << std::endl;
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
        m_program.append<Declaration>(varname, type);
        return true;
    }
    else
    {
        space();
        parseExp();  // throw an exception if it couldn't
        
        m_program.append<Definition>(varname, type, m_node);
        return true;
    }
}

bool Parser::parseExp()
{
    /*
        Trying to parse right hand side values, such as:
        - integers
        - floats
        - strings
        - booleans
        - TODO function call
        - TODO class instanciation
        - TODO method call
        - TODO operations (comparisons, additions...)
    */

    auto current = getCount();

    // parsing float before integer because float requires a '.'
    if (!parseFloat())
        back(getCount() - current + 1);
    else if (!parseInt())
        back(getCount() - current + 1);
    else if (!parseString())
        back(getCount() - current + 1);
    else if (!parseBool())
        back(getCount() - current + 1);
    // ...
    else
        error("Couldn't parse expression", "???");
    
    return true;
}

bool Parser::parseInt()
{
    std::string n = "";
    if (signedNumber(&n))
    {
        m_node = std::make_unique<Number>(std::stoi(n));
        return true;
    }
    return false;
}

bool Parser::parseFloat()
{
    std::string f = "";
    if (signedNumber(&f) && accept(IsChar('.'), &f) && number(&f))
    {
        m_node = std::make_unique<Float>(std::stof(f));
        return true;
    }
    return false;
}

bool Parser::parseString()
{
    std::string s = "";

    if (accept(IsChar('"'), &s))
    {
        while (accept(IsNot('"'), &s));
        except(IsChar('"', &s));

        // remove " at the beginning and end
        s.erase(0, 1);
        s.erase(s.end() - 1);

        m_node = std::make_unique<String>(s);

        return true;
    }
    return false;
}

bool Parser::parseBool()
{
    std::string s = "";

    if (!name(&s))
        return false;
    
    if (s == "false")
        m_node = std::make_unique<Bool>(false);
    else if (s == "true")
        m_node = std::make_unique<Bool>(true);
    else
        error("Expected 'true' or 'false'", s);
    
    return true;
}