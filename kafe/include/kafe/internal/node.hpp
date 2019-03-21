#ifndef kafe_internal_node_hpp
#define kafe_internal_node_hpp

#include <string>
#include <vector>
#include <memory>

namespace kafe
{
    namespace internal
    {
        // base for all the nodes of the AST (Abstract Syntax Tree) of a Kafe program
        struct Node
        {
            Node(const std::string& nodename);

            const std::string nodename;
        };

        using NodePtr = std::unique_ptr<Node>;
        using NodePtrList = std::vector<NodePtr>;

        // basic class to hold all the sub-nodes
        struct Program : public Node
        {
            Program();

            /*
                To add a Node to a program:
                program.append<NodeType>(arg1, arg2);
            */
            template <typename T, typename Args...>
            void append(Args&&... args)
            {
                children.push_back(
                    std::move(
                        std::make_unique<T>(std::forward(args)...);
                        )
                    );
            }

            NodePtrList children;
        };

        // Node handling declaration, i.e. varname: type
        struct Declaration : public Node
        {
            Declaration(const std::string& varname, const std::string& type);

            const std::string varname;
            const std::string type;
        };

        // Node handling definition, i.e. varname: type = value
        struct Definition : public Node
        {
            Definition(const std::string& varname, const std::string& type, NodePtr value);

            const std::string varname;
            const std::string type;
            NodePtr value;
        };

        // Node handling constants: cst name: type = value
        struct ConstDef : public Node
        {
            ConstDef(const std::string& varname, const std::string& type, NodePtr value);

            const std::string varname;
            const std::string type;
            NodePtr value;
        };

        // Node handling function: fun name(arg1: A, arg2: B) -> C *body* end
        struct Function : public Node
        {
            Function(const std::string& name, NodePtrList arguments, const std::string& type, NodePtrList body);

            const std::string name;
            NodePtrList arguments;  // should be a vector of declaration
            const std::string type;
            NodePtrList body;
        };

        struct Class : public Node
        {
            Class(const std::string& name, NodePtr constructor, NodePtrList methods, NodePtrList attributes);

            const std::string name;
            NodePtr constructor;  // should be a function
            NodePtrList methods;  // should be a vector of function
            NodePtrList attributes;  // should be a vector of definition/declaration
        };

        /*
            Node handling:

            if cond then
                stuff here
            elif cond2 then
                stuff here
            elif cond3 then
                stuff here
            ...
            else
                stuff
            end
        */
        struct IfClause : public Node
        {
            IfClause(NodePtr condition, NodePtrList body, NodePtrList elifClause, NodePtrList elseClause);

            NodePtr condition;
            NodePtrList body;
            NodePtrList elifClause;  // should be a vector of ifclause (acting as elifs)
            NodePtrList elseClause;  // contains the body of the else clause
        };

        /*
            Node handling:

            while cond do
                stuff here
            end
        */
        struct WhileLoop : public Node
        {
            WhileLoop(NodePtr condition, NodePtrList body);

            NodePtr condition;
            NodePtrList body;
        };

        struct Integer : public Node
        {
            Number(int n);

            const int value;
        };

        struct Float : public Node
        {
            Float(float f);

            const float value;
        };

        struct String : public Node
        {
            String(const std::string& s);

            const std::string value;
        };

        struct Bool : public Node
        {
            Bool(bool b);

            const bool value;
        };
    }
}

#endif