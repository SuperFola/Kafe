#include <kafe/internal/node.hpp>

using namespace kafe::internal;

inline void printIndent(std::ostream& os, std::size_t i)
{
    for (std::size_t j=0; j < i; ++j)
        os << "    ";
}

// ---------------------------

Node::Node(const std::string& nodename) :
    nodename(nodename)
{}

// ---------------------------

Program::Program() :
    Node("program")
{}

void Program::toString(std::ostream& os, std::size_t indent)
{
    os << "(Program";
    for (auto& node: children)
    {
        os << "\n";
        node->toString(os, indent + 1);
    }
    os << "\n)";
}

// ---------------------------

Declaration::Declaration(const std::string& varname, const std::string& type) :
    varname(varname), type(type)
    , Node("decl")
{}

void Declaration::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Declaration\n";
    printIndent(os, indent + 1);     os << "(VarName " << varname << ")\n";
    printIndent(os, indent + 1);     os << "(Type " << type << ")\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

Definition::Definition(const std::string& varname, const std::string& type, NodePtr value) :
    varname(varname), type(type), value(std::move(value))
    , Node("def")
{}

void Definition::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Definition\n";
    printIndent(os, indent + 1);     os << "(VarName " << varname << ")\n";
    printIndent(os, indent + 1);     os << "(Type " << type << ")\n";
                                     value->toString(os, indent + 1); os << "\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

ConstDef::ConstDef(const std::string& varname, const std::string& type, NodePtr value) :
    varname(varname), type(type), value(std::move(value))
    , Node("const def")
{}

void ConstDef::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(ConstDef\n";
    printIndent(os, indent + 1);     os << "(VarName " << varname << ")\n";
    printIndent(os, indent + 1);     os << "(Type " << type << ")\n";
                                     value->toString(os, indent + 1); os << "\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

Function::Function(const std::string& name, NodePtrList arguments, const std::string& type, NodePtrList body) :
    name(name), arguments(std::move(arguments)), type(type), body(std::move(body))
    , Node("function")
{}

void Function::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Function\n";
    printIndent(os, indent + 1);     os << "(Name " << name << ")\n";
    printIndent(os, indent + 1);     os << "(Args";
    for (auto& node: arguments)
    {
        os << "\n";
        node->toString(os, indent + 2);
    }
    if (arguments.size() > 0)
    {
        os << "\n";
        printIndent(os, indent + 1);
    }
    os << ")\n";
    printIndent(os, indent + 1);     os << "(Type " << type << ")\n";
    printIndent(os, indent + 1);     os << "(Body";
    for (auto& node: body)
    {
        os << "\n";
        node->toString(os, indent + 2);
    }
    if (body.size() > 0)
    {
        os << "\n";
        printIndent(os, indent + 1);
    }
    os << ")\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

Class::Class(const std::string& name, NodePtr constructor, NodePtrList body) :
    name(name), constructor(std::move(constructor)), body(std::move(body))
    , Node("class")
{}

void Class::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Class\n";
    printIndent(os, indent + 1);     os << "(Name " << name << ")\n";
    constructor->toString(os, indent + 1);
    printIndent(os, indent + 1);     os << "(Body";
    for (auto& node: body)
    {
        os << "\n";
        node->toString(os, indent + 2);
    }
    if (body.size() > 0)
    {
        os << "\n";
        printIndent(os, indent + 1);
    }
    os << ")\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

IfClause::IfClause(NodePtr condition, NodePtrList body, NodePtrList elifClause, NodePtrList elseClause) :
    condition(std::move(condition)), body(std::move(body)), elifClause(std::move(elifClause)), elseClause(std::move(elseClause))
    , Node("if")
{}

void IfClause::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(IfClause)";
}

// ---------------------------

WhileLoop::WhileLoop(NodePtr condition, NodePtrList body) :
    condition(std::move(condition)), body(std::move(body))
    , Node("while")
{}

void WhileLoop::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(WhileLoop)";
}

// ---------------------------

Integer::Integer(int n) :
    value(n)
    , Node("integer")
{}

void Integer::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Integer " << value << ")";
}

// ---------------------------

Float::Float(float f) :
    value(f)
    , Node("float")
{}

void Float::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Float " << value << ")";
}

// ---------------------------

String::String(const std::string& s) :
    value(s)
    , Node("string")
{}

void String::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(String \"" << value << "\")";
}

// ---------------------------

Bool::Bool(bool b) :
    value(b)
    , Node("bool")
{}

void Bool::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Bool " << (value ? "true" : "false") << ")";
}

// ---------------------------

VarUse::VarUse(const std::string& name) :
    name(name)
    , Node("var use")
{}

void VarUse::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(VarUse " << name << ")";
}

// ---------------------------

Operator::Operator(const std::string& name) :
    name(name)
    , Node("operator")
{}

void Operator::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(Operator " << name << ")";
}

// ---------------------------

OperationsList::OperationsList(NodePtrList operations) :
    operations(std::move(operations))
    , Node("op list")
{}

void OperationsList::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(OperationsList";
    for (auto& node: operations)
    {
        os << "\n";
        node->toString(os, indent + 1);
    }
    if (operations.size() > 0)
        os << "\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

FunctionCall::FunctionCall(const std::string& name, NodePtrList arguments) :
    name(name), arguments(std::move(arguments))
    , Node("function call")
{}

void FunctionCall::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(FunctionCall\n";
    printIndent(os, indent + 1);     os << "(Name " << name << ")\n";
    printIndent(os, indent + 1);     os << "(Args";
    for (auto& node: arguments)
    {
        os << "\n";
        node->toString(os, indent + 2);
    }
    if (arguments.size() > 0)
    {
        os << "\n";
        printIndent(os, indent + 1);
    }
    os << ")\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

MethodCall::MethodCall(const std::string& classname, const std::string& funcname, NodePtrList arguments) :
    classname(classname), funcname(funcname), arguments(std::move(arguments))
    , Node ("method call")
{}

void MethodCall::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(MethodCall\n";
    printIndent(os, indent + 1);     os << "(ClassName " << classname << ")\n";
    printIndent(os, indent + 1);     os << "(FuncName " << funcname << ")\n";
    printIndent(os, indent + 1);     os << "(Args";
    for (auto& node: arguments)
    {
        os << "\n";
        node->toString(os, indent + 2);
    }
    if (arguments.size() > 0)
    {
        os << "\n";
        printIndent(os, indent + 1);
    }
    os << ")\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

ClassInstanciation::ClassInstanciation(const std::string& name, NodePtrList arguments) :
    name(name), arguments(std::move(arguments))
    , Node("class instanciation")
{}

void ClassInstanciation::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(ClassInstanciation\n";
    printIndent(os, indent + 1);     os << "(Name " << name << ")\n";
    printIndent(os, indent + 1);     os << "(Args";
    for (auto& node: arguments)
    {
        os << "\n";
        node->toString(os, indent + 2);
    }
    if (arguments.size() > 0)
    {
        os << "\n";
        printIndent(os, indent + 1);
    }
    os << ")\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

ClsConstructor::ClsConstructor(const std::string& name, NodePtrList body) :
    name(name), body(std::move(body))
    , Node("class constructor")
{}

void ClsConstructor::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(ClassConstructor\n";
    printIndent(os, indent + 1);     os << "(Name " << name << ")\n";
    printIndent(os, indent + 1);     os << "(Body";
    for (auto& node: body)
    {
        os << "\n";
        node->toString(os, indent + 2);
    }
    if (body.size() > 0)
    {
        os << "\n";
        printIndent(os, indent + 1);
    }
    os << ")\n";
    printIndent(os, indent);     os << ")";
}

// ---------------------------

End::End() :
    Node("end")
{}

void End::toString(std::ostream& os, std::size_t indent)
{
    printIndent(os, indent);     os << "(End)";
}