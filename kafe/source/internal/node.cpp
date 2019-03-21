#include <kafe/internal/node.hpp>

using namespace kafe::internal;

Node::Node(const std::string& nodename) :
    nodename(nodename)
{}

// ---------------------------

Program::Program() :
    Node("program")
{}

// ---------------------------

Declaration::Declaration(const std::string& varname, const std::string& type) :
    varname(varname), type(type)
    , Node("decl")
{}

// ---------------------------

Definition::Definition(const std::string& varname, const std::string& type, NodePtr value) :
    varname(varname), type(type), value(std::move(value))
    , Node("def")
{}

// ---------------------------

ConstDef::ConstDef(const std::string& varname, const std::string& type, NodePtr value) :
    varname(varname), type(type), value(std::move(value))
    , Node("const def")
{}

// ---------------------------

Function::Function(const std::string& name, NodePtrList arguments, const std::string& type, NodePtrList body) :
    name(name), arguments(std::move(arguments)), type(type), body(std::move(body))
    , Node("function")
{}

// ---------------------------

Class::Class(const std::string& name, NodePtr constructor, NodePtrList methods, NodePtrList attributes) :
    name(name), constructor(std::move(constructor)), methods(std::move(methods)), attributes(std::move(attributes))
    , Node("class")
{}

// ---------------------------

IfClause::IfClause(NodePtr condition, NodePtrList body, NodePtrList elifClause, NodePtrList elseClause) :
    condition(std::move(condition)), body(std::move(body)), elifClause(std::move(elifClause)), elseClause(std::move(elseClause))
    , Node("if")
{}

// ---------------------------

WhileLoop::WhileLoop(NodePtr condition, NodePtrList body) :
    condition(std::move(condition)), body(std::move(body))
    , Node("while")
{}

// ---------------------------

Number::Number(int n) :
    value(n)
    , Node("integer")
{}

// ---------------------------

Float::Float(float f) :
    value(f)
    , Node("float")
{}

// ---------------------------

String::String(const std::string& s) :
    value(s)
    , Node("string")
{}