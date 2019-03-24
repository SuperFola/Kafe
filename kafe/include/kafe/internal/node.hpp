#ifndef kafe_internal_node_hpp
#define kafe_internal_node_hpp

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace kafe
{
    namespace internal
    {
        // base for all the nodes of the AST (Abstract Syntax Tree) of a Kafe program
        struct Node
        {
            Node(const std::string& nodename);

            const std::string nodename;

            virtual void toString(std::ostream& os, std::size_t indent) = 0;
        };

        using NodePtr = std::shared_ptr<Node>;
        using NodePtrList = std::vector<NodePtr>;

        // basic class to hold all the sub-nodes
        struct Program : public Node
        {
            Program();

            /*
                To add a Node to a program:
                program.append<NodeType>(arg1, arg2);
            */
            template <typename T, typename... Args>
            void append(Args&&... args)
            {
                children.push_back(
                    std::move(
                        std::make_shared<T>(std::forward<Args>(args)...)
                        )
                    );
            }

            NodePtrList children;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        // Node handling declaration, i.e. varname: type
        struct Declaration : public Node
        {
            Declaration(const std::string& varname, const std::string& type);

            const std::string varname;
            const std::string type;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        // Node handling definition, i.e. varname: type = value
        struct Definition : public Node
        {
            Definition(const std::string& varname, const std::string& type, NodePtr value);

            const std::string varname;
            const std::string type;
            NodePtr value;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        // Node handling constants: cst name: type = value
        struct ConstDef : public Node
        {
            ConstDef(const std::string& varname, const std::string& type, NodePtr value);

            const std::string varname;
            const std::string type;
            NodePtr value;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        // Node handling function: fun name(arg1: A, arg2: B) -> C *body* end
        struct Function : public Node
        {
            Function(const std::string& name, NodePtrList arguments, const std::string& type, NodePtrList body);

            const std::string name;
            NodePtrList arguments;  // should be a vector of declaration
            const std::string type;
            NodePtrList body;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct Class : public Node
        {
            Class(const std::string& name, NodePtr constructor, NodePtrList methods, NodePtrList attributes);

            const std::string name;
            NodePtr constructor;  // should be a function
            NodePtrList methods;  // should be a vector of function
            NodePtrList attributes;  // should be a vector of definition/declaration

            virtual void toString(std::ostream& os, std::size_t indent);
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

            virtual void toString(std::ostream& os, std::size_t indent);
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

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct Integer : public Node
        {
            Integer(int n);

            const int value;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct Float : public Node
        {
            Float(float f);

            const float value;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct String : public Node
        {
            String(const std::string& s);

            const std::string value;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct Bool : public Node
        {
            Bool(bool b);

            const bool value;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct VarUse : public Node
        {
            VarUse(const std::string& name);

            const std::string name;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct Operator : public Node
        {
            Operator(const std::string& name);

            const std::string name;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct OperationsList : public Node
        {
            OperationsList(NodePtrList operations);

            NodePtrList operations;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct FunctionCall : public Node
        {
            FunctionCall(const std::string& name, NodePtrList arguments);

            const std::string name;
            NodePtrList arguments;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct MethodCall : public Node
        {
            MethodCall(const std::string& classname, const std::string& funcname, NodePtrList arguments);

            const std::string classname;
            const std::string funcname;
            NodePtrList arguments;

            virtual void toString(std::ostream& os, std::size_t indent);
        };

        struct End : public Node
        {
            End();

            virtual void toString(std::ostream& os, std::size_t indent);
        };
    }
}

#endif