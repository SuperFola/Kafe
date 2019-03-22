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

            virtual std::ostream& toString(std::ostream& os) = 0;
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

            virtual std::ostream& toString(std::ostream& os);
        };

        // Node handling declaration, i.e. varname: type
        struct Declaration : public Node
        {
            Declaration(const std::string& varname, const std::string& type);

            const std::string varname;
            const std::string type;

            virtual std::ostream& toString(std::ostream& os);
        };

        // Node handling definition, i.e. varname: type = value
        struct Definition : public Node
        {
            Definition(const std::string& varname, const std::string& type, NodePtr value);

            const std::string varname;
            const std::string type;
            NodePtr value;

            virtual std::ostream& toString(std::ostream& os);
        };

        // Node handling constants: cst name: type = value
        struct ConstDef : public Node
        {
            ConstDef(const std::string& varname, const std::string& type, NodePtr value);

            const std::string varname;
            const std::string type;
            NodePtr value;

            virtual std::ostream& toString(std::ostream& os);
        };

        // Node handling function: fun name(arg1: A, arg2: B) -> C *body* end
        struct Function : public Node
        {
            Function(const std::string& name, NodePtrList arguments, const std::string& type, NodePtrList body);

            const std::string name;
            NodePtrList arguments;  // should be a vector of declaration
            const std::string type;
            NodePtrList body;

            virtual std::ostream& toString(std::ostream& os);
        };

        struct Class : public Node
        {
            Class(const std::string& name, NodePtr constructor, NodePtrList methods, NodePtrList attributes);

            const std::string name;
            NodePtr constructor;  // should be a function
            NodePtrList methods;  // should be a vector of function
            NodePtrList attributes;  // should be a vector of definition/declaration

            virtual std::ostream& toString(std::ostream& os);
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

            virtual std::ostream& toString(std::ostream& os);
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

            virtual std::ostream& toString(std::ostream& os);
        };

        struct Integer : public Node
        {
            Integer(int n);

            const int value;

            virtual std::ostream& toString(std::ostream& os);
        };

        struct Float : public Node
        {
            Float(float f);

            const float value;

            virtual std::ostream& toString(std::ostream& os);
        };

        struct String : public Node
        {
            String(const std::string& s);

            const std::string value;

            virtual std::ostream& toString(std::ostream& os);
        };

        struct Bool : public Node
        {
            Bool(bool b);

            const bool value;

            virtual std::ostream& toString(std::ostream& os);
        };

        struct End : public Node
        {
            End();

            virtual std::ostream& toString(std::ostream& os);
        };
    }
}

#endif