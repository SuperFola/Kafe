#include <kafe/parser.hpp>
#include <iostream>

using namespace kafe;

Parser::Parser() :
    m_code("")
{}

Parser::~Parser()
{}

void Parser::feed(const std::string& code)
{
    m_code += code;
}

void Parser::parse()
{
    if (auto decl = parseDeclaration(m_code))
    {
        std::cout << "parsed declaration" << std::endl;
    }
    //else
    //    std::cout << "couldn't parse declaration" << std::endl;
}

std::optional<Declaration> Parser::parseDeclaration(const std::string& code)
{
    /*
        We want to be able to parse this kind of code:
        var_name:type
        var_name:type=value
        With or without spaces around ':' and '='
    */

    auto it = code.begin();

    std::string var_name = "";
    for (; it != code.end() && *it != ' ' && *it != ':' && *it != '\t'; ++it)
    {
        // will character isn't a space or a colon, retrieve it
        // because it belongs to the var_name
        var_name += *it;
    }

    // skipping spaces and ':'
    bool had_colon = false;
    while (!had_colon)
    {
        if (it == code.end())
        {
            std::cout << "reached end of string. aborting" << std::endl;
            return std::nullopt;
        }

        // skip spaces
        if (*it == ' ' || *it == '\t')
            ++it;
        else if (*it == ':')  // check if we got the colon
            had_colon = true;
    }
    ++it;

    // if we are here, we found the colon
    // skip remaining spaces
    while (true)
    {
        if (it == code.end())
        {
            std::cout << "reached end of string. aborting" << std::endl;
            return std::nullopt;
        }

        if (*it == ' ' || *it == '\t')
            ++it;
        else
            break;
    }

    // getting type_name now
    std::string type_name = "";
    for (; it != code.end() && *it != ' ' && *it != '\t' && *it != '\n' && *it != '\r' && *it != ':'; ++it)
    {
        // will character isn't a space or a colon, retrieve it
        // because it belongs to the type_name
        type_name += *it;
    }

    if (it != code.end())
    {
        // we expect a '=', possibly surrounded by spaces
        bool had_assign = false;
        while (!had_assign)
        {
            if (it == code.end())
            {
                std::cout << "reached end of string. aborting" << std::endl;
                return std::nullopt;
            }

            // skip spaces
            if (*it == ' ' || *it == '\t')
                ++it;
            else if (*it == '=')  // check if we got the assignment operator
                had_assign = true;
        }
        ++it;

        // if we are here, we found the assignment operator
        // skip remaining spaces
        while (true)
        {
            if (it == code.end())
            {
                std::cout << "reached end of string. aborting" << std::endl;
                return std::nullopt;
            }

            if (*it == ' ' || *it == '\t')
                ++it;
            else
                break;
        }

        std::string value = "";
        // currently we will say everything after is a value
        for (; it != code.end(); ++it)
            value += *it;
        
        std::cout << "parsed successfully: VAR(" << var_name << "), TYPE(" << type_name << ")"
            << ", VALUE(" << value << ")" << std::endl;
        
        return {};  // temp. remove it. generate a declaration. thanks
    }
    else
    {
        std::cout << "parsed successfully: VAR(" << var_name << "), TYPE(" << type_name << ")" << std::endl;
        
        return {};  // temp. remove it. generate a declaration. thanks
    }
}