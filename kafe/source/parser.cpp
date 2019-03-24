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
            std::cout << "[Parser] Error, couldn't recognize instruction" << std::endl;
            break;
        }
        else
            m_program.children.push_back(std::move(inst.value()));
    }
}

void Parser::ASTtoString(std::ostream& os)
{
    m_program.toString(os, /* default indentation level */ 0);
}

bool Parser::operator_(std::string* s)
{
    if (accept(IsNot(IsChar(' ')), s))
    {
        while (accept(IsNot(IsChar(' ')), s));
        return true;
    }
    return false;
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
    
    // class Name ... end
    if (auto inst = parseClass())
        return inst;
    else
        back(getCount() - current + 1);
    
    // token 'end' closing a block
    if (auto inst = parseEnd())
        return inst;
    else
        back(getCount() - current + 1);
    

    // function/method calls as expression, not as instruction!
    // FunctionCall and MethodCall should be the last ones of the list
    if (auto inst = parseFunctionCall())
        error("Function calls as instructions are forbidden", "");
    else
        back(getCount() - current + 1);
    
    if (auto inst = parseMethodCall())
        error("Method calls as instructions are forbidden", "");
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
    if (!accept(IsChar(':')))
        return {};
    space();

    std::string type = "";
    if (!name(&type))
        error("Expected type name for declaration", type);
    
    space();
    // checking for value (optional)
    if (!accept(IsChar('=')))
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
        error("Expected constant name", varname);
    
    space();
    // : after varname and before type is mandatory
    except(IsChar(':'));
    space();

    std::string type = "";
    if (!name(&type))
        error("Expected type name for constant definition", type);
    
    space();
    // checking for value
    except(IsChar('='));
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
        - function call
        - TODO class instanciation
        - method call
        - operations (comparisons, additions...)
    */

    auto current = getCount();

    // parsing operations before anything else because it must use the other parsers
    if (auto exp = parseOperation())
        return exp;
    else
        back(getCount() - current + 1);

    if (auto exp = parseSingleExp())
        return exp;
    else
        back(getCount() - current + 1);

    error("Couldn't parse expression", "");
    return {};  // to avoid warnings
}

MaybeNodePtr Parser::parseOperation()
{
    /*
        Trying to parse operations such as
        (1 + 2)
        (1 / (2 + 3))

        Get current token: it must be a '('
        If it's not => quit
        Otherwise, get token[n+1], it must an expression
        Token[n+2] must be an operator:
            +, -, *, /
            <<, >>, ~
            and, or, not
            ==, !=, <, <=, >, >=
        Parsing end when:
            - the expression couldn't be parsed (VarUse being a keyword can break the whole thing)
            - there was no error and we found the matching ')'
        
        TODO This parser should also do a "shunting yard"
    */

    space();

    if (!accept(IsChar('(')))
        return {};
    
    // parse expressions
    NodePtrList operations;
    while (true)
    {
        space();

        // check for end of operation
        if (accept(IsChar(')')))
            break;

        // getting prefix operator
        auto current = getCount();
        if (accept(IsMinus))
            operations.push_back(std::make_shared<Operator>("-"));
        else
            back(getCount() - current + 1);
        
        if (accept(IsChar('~')))
            operations.push_back(std::make_shared<Operator>("~"));
        else
            back(getCount() - current + 1);
        
        std::string token = "";
        if (name(&token) && token == "not")
            operations.push_back(std::make_shared<Operator>("not"));
        else
            back(getCount() - current + 1);
        
        // get operand
        MaybeNodePtr exp = parseExp();  // throw an error if it couldn't
        operations.push_back(exp.value());

        // check for end of operation
        if (accept(IsChar(')')))
            break;
        
        space();

        std::string op = "";
        if (!operator_(&op))
            return {};
        if (!isOperator(op))
            return {};
        
        operations.push_back(std::make_shared<Operator>(op));
    }

    if (operations.size() == 0)
        error("Expected operations inside block", "");

    return std::make_shared<OperationsList>(operations);
}

MaybeNodePtr Parser::parseSingleExp()
{
    auto current = getCount();

    // parsing float before integer because float requires a '.'
    if (auto exp = parseFloat())  // 1.5
        return exp;
    else
        back(getCount() - current + 1);

    if (auto exp = parseInt())  // 42
        return exp;
    else
        back(getCount() - current + 1);

    if (auto exp = parseString())  // "hello world"
        return exp;
    else
        back(getCount() - current + 1);

    if (auto exp = parseBool())  // true
        return exp;
    else
        back(getCount() - current + 1);

    
    if (auto exp = parseFunctionCall())  // foo(42, -6.66)
        return exp;
    else
        back(getCount() - current + 1);
    
    if (auto exp = parseMethodCall())  // bar.foo(42, -6.66)
        return exp;
    else
        back(getCount() - current + 1);
    
    // must the last one, otherwise it would try to parse function/method calls
    if (auto exp = parseVarUse())  // varname
        return exp;
    else
        back(getCount() - current + 1);
    
    error("Couldn't parse single expression", "");
    return {};  // to avoid warnings
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
    
    return {};
}

MaybeNodePtr Parser::parseFunctionCall()
{
    /*
        Trying to parse stuff like this:

        main(1, "hello")
        foo(true)
        doStuff()
    */

    space();

    // getting the name of the function
    std::string funcname = "";
    if (!name(&funcname))
        return {};
    
    space();
    
    // getting the arguments
    NodePtrList arguments;
    if (accept(IsChar('(')))
    {
        while (true)
        {
            // eat the trailing white space
            space();

            // check if end of arguments
            if (accept(IsChar(')')))
                break;

            // find argument
            MaybeNodePtr inst = parseExp();  // throw an error if it couldn't
            arguments.push_back(inst.value());

            space();

            // check for ',' -> other arguments
            if (accept(IsChar(',')))
                continue;
        }

        return std::make_shared<FunctionCall>(funcname, arguments);
    }
    return {};
}

MaybeNodePtr Parser::parseMethodCall()
{
    /*
        Trying to parse stuff like this:

        object.main(1, "hello")
        player.foo(true)
        you.doStuff()
    */

    space();

    // getting the name of the object
    std::string objectname = "";
    if (!name(&objectname))
        return {};
    
    if (!accept(IsChar('.')))  // '.' between object name and method name
        return {};
    
    // getting function name
    std::string funcname = "";
    if (!name(&funcname))
        error("Expecting a method name after '" + objectname + ".'", funcname);
    
    space();
    
    // getting the arguments
    NodePtrList arguments;
    if (accept(IsChar('(')))
    {
        while (true)
        {
            // eat the trailing white space
            space();

            // check if end of arguments
            if (accept(IsChar(')')))
                break;

            // find argument
            MaybeNodePtr inst = parseExp();  // throw an error if it couldn't
            arguments.push_back(inst.value());

            space();

            // check for ',' -> other arguments
            if (accept(IsChar(',')))
                continue;
        }

        return std::make_shared<MethodCall>(objectname, funcname, arguments);
    }
    return {};
}

MaybeNodePtr Parser::parseVarUse()
{
    /*
        Trying to parse things such as

        x: int = varname
        ~~~~~~~~~^^^^^^^
    */

    std::string varname = "";
    if (!name(&varname))
        return {};
    
    // checking if varname is a keyword
    if (isKeyword(varname))
        return {};
    
    return std::make_shared<VarUse>(varname);
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
        error("Expected function name", funcname);
    
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
            if (!except(IsChar(':')))
                error("Expected ':' after argument name and before type name", "");
            space();

            std::string type = "";
            if (!name(&type))
                error("Expected type name for argument in function definition", type);
            
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
        error("Expected return type for function definition", type);
    
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
            error("Expected valid instruction for body of function definition", "");
    }

    return std::make_shared<Function>(funcname, arguments, type, body);
}

MaybeNodePtr Parser::parseClass()
{
    /*
        Trying to parse class definition:

        class Name
            // only one constructor!
            Name(arg: type, ...)
                ...  // no ret here!
            end

            fun method(arg: type, ...) -> ret_type
                ...
                ret object_of_type_ret_type
            end

            variable: type
            variable: type = value
        end
    */

    return {};
}