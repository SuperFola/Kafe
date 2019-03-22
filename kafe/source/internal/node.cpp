#include <kafe/internal/node.hpp>

using namespace kafe::internal;

Node::Node(const std::string& nodename) :
    nodename(nodename)
{}

// ---------------------------

Program::Program() :
    Node("program")
{}

std::ostream& Program::toString(std::ostream& os)
{
    os << "(Program";
    for (auto& node: children)
    {
        os << " ";
        node->toString(os);
    }
    os << ")";
    return os;
}

// ---------------------------

Declaration::Declaration(const std::string& varname, const std::string& type) :
    varname(varname), type(type)
    , Node("decl")
{}

std::ostream& Declaration::toString(std::ostream& os)
{
    os << "(Declaration (VarName " << varname << ") (Type "
        << type << "))";
    return os;
}

// ---------------------------

Definition::Definition(const std::string& varname, const std::string& type, NodePtr value) :
    varname(varname), type(type), value(std::move(value))
    , Node("def")
{}

std::ostream& Definition::toString(std::ostream& os)
{
    os << "(Definition (VarName " << varname << ") (Type "
        << type << ") ";
    if (value)
        value->toString(os);
    else
        os << "NO VALUE";
    os << ")";
    return os;
}

// ---------------------------

ConstDef::ConstDef(const std::string& varname, const std::string& type, NodePtr value) :
    varname(varname), type(type), value(std::move(value))
    , Node("const def")
{}

std::ostream& ConstDef::toString(std::ostream& os)
{
    os << "(ConstDef (VarName " << varname << ") (Type "
        << type << ") ";
    value->toString(os);
    os << ")";
    return os;
}

// ---------------------------

Function::Function(const std::string& name, NodePtrList arguments, const std::string& type, NodePtrList body) :
    name(name), arguments(std::move(arguments)), type(type), body(std::move(body))
    , Node("function")
{}

std::ostream& Function::toString(std::ostream& os)
{
    os << "(Function)";
    return os;
}

// ---------------------------

Class::Class(const std::string& name, NodePtr constructor, NodePtrList methods, NodePtrList attributes) :
    name(name), constructor(std::move(constructor)), methods(std::move(methods)), attributes(std::move(attributes))
    , Node("class")
{}

std::ostream& Class::toString(std::ostream& os)
{
    os << "(Class)";
    return os;
}

// ---------------------------

IfClause::IfClause(NodePtr condition, NodePtrList body, NodePtrList elifClause, NodePtrList elseClause) :
    condition(std::move(condition)), body(std::move(body)), elifClause(std::move(elifClause)), elseClause(std::move(elseClause))
    , Node("if")
{}

std::ostream& IfClause::toString(std::ostream& os)
{
    os << "(IfClause)";
    return os;
}

// ---------------------------

WhileLoop::WhileLoop(NodePtr condition, NodePtrList body) :
    condition(std::move(condition)), body(std::move(body))
    , Node("while")
{}

std::ostream& WhileLoop::toString(std::ostream& os)
{
    os << "(WhileLoop)";
    return os;
}

// ---------------------------

Integer::Integer(int n) :
    value(n)
    , Node("integer")
{}

std::ostream& Integer::toString(std::ostream& os)
{
    os << "(Integer " << value << ")";
    return os;
}

// ---------------------------

Float::Float(float f) :
    value(f)
    , Node("float")
{}

std::ostream& Float::toString(std::ostream& os)
{
    os << "(Float " << value << ")";
    return os;
}

// ---------------------------

String::String(const std::string& s) :
    value(s)
    , Node("string")
{}

std::ostream& String::toString(std::ostream& os)
{
    os << "(String " << value << ")";
    return os;
}

// ---------------------------

Bool::Bool(bool b) :
    value(b)
    , Node("bool")
{}

std::ostream& Bool::toString(std::ostream& os)
{
    os << "(Bool " << (value ? "true" : "false") << ")";
    return os;
}