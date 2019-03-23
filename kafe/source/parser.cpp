#include <kafe/parser.hpp>

using namespace kafe;
using namespace kafe::internal;

Parser::Parser(const std::string& code) :
    internal::ParserCombinators(code)
{}

Parser::~Parser()
{}

void Parser::parse()
{
    // parse until the end of the string
    while (!isEOF())
    {
        MaybeNodePtr inst = parseInstruction();
        if (!inst)
        {
            std::cout << "Parse error" << std::endl;
            break;
        }
        else
            m_program.children.push_back(std::move(inst.value()));
    }
}

void Parser::ASTtoString(std::ostream& os)
{
    m_program.toString(os);
}

MaybeNodePtr Parser::parseInstruction()
{
    // save current position in buffer to be able to go back if needed
    auto current = getCount();

    // x:type, x:type=value
    if (auto inst = parseDeclaration())
        return inst;
    else
        back(getCount() - current + 1);
    
    // cst x:type=value
    if (auto inst = parseConstDef())
        return inst;
    else
        back(getCount() - current + 1);
    
    // fun name(arg:type, ...) -> type {body} end
    if (auto inst = parseFunction())
        return inst;
    else
        back(getCount() - current + 1);
    
    if (auto inst = parseEnd())
        return inst;
    else
        back(getCount() - current + 1);
    
    return {};
}

MaybeNodePtr Parser::parseDeclaration()
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
        return {};
    
    space();
    // : after varname and before type is mandatory
    if (!accept(internal::IsChar(':')))
        return {};
    space();

    std::string type = "";
    if (!name(&type))
        return {};
    
    space();
    // checking for value (optional)
    if (!accept(internal::IsChar('=')))
        return std::make_shared<Declaration>(varname, type);
    else
    {
        space();

        // throw an exception if it couldn't
        MaybeNodePtr exp = parseExp();
        return std::make_shared<Definition>(varname, type, exp.value());
    }
}

MaybeNodePtr Parser::parseConstDef()
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
        return {};
    if (const_qualifier != "cst")
        return {};
    
    space();

    std::string varname = "";
    if (!name(&varname))
        return {};
    
    space();
    // : after varname and before type is mandatory
    except(internal::IsChar(':'));
    space();

    std::string type = "";
    if (!name(&type))
        return {};
    
    space();
    // checking for value
    except(internal::IsChar('='));
    space();
    
    // throw an exception if it couldn't
    MaybeNodePtr exp = parseExp();
    return std::make_shared<ConstDef>(varname, type, exp.value());
}

MaybeNodePtr Parser::parseExp()
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
    if (auto exp = parseFloat())
        return exp;
    else
        back(getCount() - current + 1);

    if (auto exp = parseInt())
        return exp;
    else
        back(getCount() - current + 1);

    if (auto exp = parseString())
        return exp;
    else
        back(getCount() - current + 1);

    if (auto exp = parseBool())
        return exp;
    else
        back(getCount() - current + 1);

    error("Couldn't parse expression", "???");
}

MaybeNodePtr Parser::parseInt()
{
    std::string n = "";
    if (signedNumber(&n))
        return std::make_shared<Integer>(std::stoi(n));
    return {};
}

MaybeNodePtr Parser::parseFloat()
{
    std::string f = "";
    if (signedNumber(&f) && accept(IsChar('.'), &f) && number(&f))
        return std::make_shared<Float>(std::stof(f));
    return {};
}

MaybeNodePtr Parser::parseString()
{
    std::string s = "";

    if (accept(IsChar('"'), &s))
    {
        while (accept(IsNot(IsChar('"')), &s));
        except(IsChar('"'), &s);

        // remove " at the beginning and end
        s.erase(0, 1);
        s.erase(s.end() - 1);

        return std::make_shared<String>(s);
    }
    return {};
}

MaybeNodePtr Parser::parseBool()
{
    std::string s = "";

    if (!name(&s))
        return {};
    
    if (s == "false")
        return std::make_shared<Bool>(false);
    else if (s == "true")
        return std::make_shared<Bool>(true);
    else
        error("Expected 'true' or 'false'", s);
    
    return {};
}

MaybeNodePtr Parser::parseEnd()
{
    /*
        Trying to parse 'end' tokens
    */

    space();

    std::string keyword = "";
    if (!name(&keyword))
        return {};
    if (keyword != "end")
        return {};
    
    return std::make_shared<End>();
}

MaybeNodePtr Parser::parseFunction()
{
    /*
        Trying to parse functions:

        fun name(arg: type, arg2: type, ...) -> type
            code...
        end

        NB: code... can (should) include a 'ret value'
    */

    // eat trailing white spaces
    space();

    // checking for 'fun'
    std::string keyword = "";
    if (!name(&keyword))
        return {};
    if (keyword != "fun")
        return {};

    space();

    // getting name
    std::string funcname = "";
    if (!name(&funcname))
        return {};
    
    space();

    // getting arguments (enclosed in ())
    NodePtrList arguments;
    if (except(IsChar('(')))
    {
        while (true)
        {
            // eat the trailing white space
            space();

            // check if end of arguments
            if (accept(IsChar(')')))
                break;

            std::string varname = "";
            if (!name(&varname))
                break;  // we don't have arguments
            
            space();
            // : after varname and before type is mandatory
            if (!except(internal::IsChar(':')))
                return {};
            space();

            std::string type = "";
            if (!name(&type))
                return {};
            
            space();

            // register argument
            arguments.push_back(
                std::make_shared<Declaration>(varname, type)
            );

            // check for ',' -> other arguments
            if (accept(IsChar(',')))
                continue;
        }
    }

    space();
    // need the full '->'
    except(IsChar('-')); except(IsChar('>'));
    space();

    // getting function type
    std::string type = "";
    if (!name(&type))
        return {};
    
    // getting the body
    NodePtrList body;
    while (true)
    {
        MaybeNodePtr inst = parseInstruction();

        // after getting the instruction, check if it's valid
        if (inst)
        {
            // if we found a 'end' token, stop
            if (inst.value()->nodename == "end")
                break;
            body.push_back(inst.value());
        }
        else
            return {};
    }

    return std::make_shared<Function>(funcname, arguments, type, body);
}