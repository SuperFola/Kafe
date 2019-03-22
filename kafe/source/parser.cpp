#include <kafe/parser.hpp>

using namespace kafe;
using namespace kafe::internal;

Parser::Parser(const std::string& code) :
    internal::ParserCombinators(code)
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
}

void Parser::ASTtoString(std::ostream& os)
{
    m_program.toString(os);
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
        
        m_program.append<Definition>(varname, type, std::move(m_node));
        return true;
    }
}

bool Parser::parseConstDef()
{
    /*
        Trying to parse constant definitions:

        cst var : type = value
    */

    // eat the trailing white space
    space();

    // checking if 'cst' is present
    std::string const_qualifier = "";
    if (!name(&const_qualifier))
        return false;
    if (const_qualifier != "cst")
        return false;
    
    space();

    std::string varname = "";
    if (!name(&varname))
        return false;
    
    space();
    // : after varname and before type is mandatory
    except(internal::IsChar(':'));
    space();

    std::string type = "";
    if (!name(&type))
        return false;
    
    space();
    // checking for value
    except(internal::IsChar('='));
    space();
    
    parseExp();  // throw an exception if it couldn't
        
    m_program.append<ConstDef>(varname, type, std::move(m_node));
    return true;
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
    else
        return true;

    if (!parseInt())
        back(getCount() - current + 1);
    else
        return true;

    if (!parseString())
        back(getCount() - current + 1);
    else
        return true;

    if (!parseBool())
        back(getCount() - current + 1);
    else
        return true;

    error("Couldn't parse expression", "???");
}

bool Parser::parseInt()
{
    std::string n = "";
    if (signedNumber(&n))
    {
        m_node = std::make_unique<Integer>(std::stoi(n));
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
        while (accept(IsNot(IsChar('"')), &s));
        except(IsChar('"'), &s);

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