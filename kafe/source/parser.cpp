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
    
    // x = value
    if (auto inst = parseAssignment())
        return inst;
    else
        back(getCount() - current + 1);
    
    // fun name(arg:type, ...) -> type {body} end
    if (auto inst = parseFunction())
        return inst;
    else
        back(getCount() - current + 1);
    
    // cls Name ... end
    if (auto inst = parseClass())
        return inst;
    else
        back(getCount() - current + 1);
    
    // new Name() ... end
    if (auto inst = parseConstructor())
        return inst;
    else
        back(getCount() - current + 1);
    
    // ret value
    if (auto inst = parseRet())
        return inst;
    else
        back(getCount() - current + 1);
    
    // token 'end' closing a block
    if (auto inst = parseEnd())
        return inst;
    else
        back(getCount() - current + 1);
    

    // function/method calls as expression, not as instruction!
    if (auto inst = parseExp())
        error("Expressions as instructions are forbidden", "");
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

        if (auto exp = parseExp())
            return std::make_shared<Definition>(varname, type, exp.value());
        else
            error("Expected a valid expression for definition", "");
    }

    return {};
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
    
    if (auto exp = parseExp())
        return std::make_shared<ConstDef>(varname, type, exp.value());
    else
        error("Expected a valid expression as a value for constant definition", "");
    
    return {};
}

MaybeNodePtr Parser::parseAssignment()
{
    /*
        Trying to parse assignment, such as:

        x = 12
        x += 15
        y -= 5
        etc.
    */

    // eat the trailing white space
    space();

    std::string varname = "";
    if (!name(&varname))
        error("Expected variable name", varname);
    
    space();
    
    space();
    // we can have an operator before the '=' sign
    std::string op = "";
    if (!operator_(&op))
        return {};
    if (op != "=" && !isOperator(op))  // what is it? we don't want it
        return {};
    space();
    
    if (auto exp = parseExp())
        return std::make_shared<Assignment>(
            varname,
            exp.value(),
            isOperator(op) ? op : "="
        );
    else
        error("Expected a valid expression as a value to assign to variable", "");
    
    return {};
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

    return {};
}

MaybeNodePtr Parser::parseOperation()
{
    /*
        Trying to parse operations such as
        1 + 2
        1 / (2 + 3)

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
    
    // parse expressions
    NodePtrList operations;
    while (true)
    {
        space();

        // getting prefix operator
        auto current = getCount();
        if (accept(IsMinus) && accept(IsSpace))
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
        
        space();
        
        // get operand
        current = getCount();
        if (auto exp = parseSingleExp())
            operations.push_back(exp.value());
        else
        {
            std::cout << "???" << std::endl;
            back(getCount() - current + 1);
            break;
        }
        
        space();

        current = getCount();
        std::string op = "";
        if (!operator_(&op))
            return {};
        if (!isOperator(op))
        {
            if (operations.size() < 2)
                return {};
            
            back(getCount() - current + 1);
            break;
        }
        
        operations.push_back(std::make_shared<Operator>(op));
    }

    if (operations.size() == 0)
        return {};

    return std::make_shared<OperationsList>(operations);
}

MaybeNodePtr Parser::parseSingleExp()
{
    auto current = getCount();

    if (auto exp = parseOperationBlock())  // (1 + 2 - 4)
        return exp;
    else
        back(getCount() - current + 1);

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
    
    if (auto exp = parseClassInstanciation())  // new Stuff("hello", 12)
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

MaybeNodePtr Parser::parseOperationBlock()
{
    /*
        Trying to parse operations, but inside parens, such as:

        (1 + 2 - 5)
    */

    if (accept(IsChar('(')))
    {
        MaybeNodePtr op = parseOperation();
        if (op && accept(IsChar(')')))
            return op;
    }

    return {};
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

MaybeNodePtr Parser::parseClassInstanciation()
{
    /*
        Trying to parse class instanciation:

        new Stuff(5, 12)
    */

    space();

    std::string keyword = "";
    if (!name(&keyword))
        return {};
    if (keyword != "new")
        return {};
    
    space();

    // getting the name of the class
    std::string clsname = "";
    if (!name(&clsname))
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
            if (auto inst = parseExp())
                arguments.push_back(inst.value());
            else
                error("Expected a valid expression as class constructor argument", "");

            space();

            // check for ',' -> other arguments
            if (accept(IsChar(',')))
                continue;
        }

        return std::make_shared<ClassInstanciation>(clsname, arguments);
    }
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
            if (auto inst = parseExp())
                arguments.push_back(inst.value());
            else
                error("Expected a valid expression as function argument", "");

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
            if (auto inst = parseExp())
                arguments.push_back(inst.value());
            else
                error("Expected a valid expression as method argument", "");

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

        cls Name
            // only one constructor!
            new Name(arg: type, ...)
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

    space();

    std::string keyword = "";
    if (!name(&keyword))
        return {};
    if (keyword != "cls")
        return {};
    
    space();

    std::string clsname = "";
    if (!name(&clsname))
        error("Expected class name", clsname);
    
    space();

    bool hadconstructor = false;
    NodePtrList body;
    NodePtr constructor;
    while (true)
    {
        // first, try to get a valid instruction
        if (auto inst = parseInstruction())
        {
            if (inst.value()->nodename == "end")
                break;
            else if (inst.value()->nodename == "class constructor")
            {
                if (!hadconstructor)
                {
                    constructor = inst.value();
                    hadconstructor = true;
                    continue;
                }
                else
                    error("The constructor of a class must be unique", clsname);
            }
            else
                body.push_back(inst.value());
        }
    }

    if (!hadconstructor)
        error("Class definition must include a constructor", clsname);

    return std::make_shared<Class>(clsname, constructor, body);
}

MaybeNodePtr Parser::parseConstructor()
{
    /*
        Trying to parse constructor definitions such as:

        new Name(arg: type, ...)
            stuff...
        end
    */

    space();

    std::string keyword = "";
    if (!name(&keyword))
        return {};
    if (keyword != "new")
        return {};
    
    space();

    // getting name
    std::string constructorname = "";
    if (!name(&constructorname))
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
            error("Expected valid instruction for body of constructor definition", "");
    }

    return std::make_shared<ClsConstructor>(constructorname, arguments, body);
}

MaybeNodePtr Parser::parseRet()
{
    /*
        Trying to parse:

        ret *expression*
    */

    space();

    std::string keyword = "";
    if (!name(&keyword))
        return {};
    if (keyword != "ret")
        return {};
    
    space();

    if (auto expr = parseExp())
        return std::make_shared<Ret>(expr.value());
    else
        error("Return instruction need a valid value", "");
    
    return {};
}