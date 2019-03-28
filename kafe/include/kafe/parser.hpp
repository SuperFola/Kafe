#ifndef kafe_parser_hpp
#define kafe_parser_hpp

#include <kafe/internal/parser.hpp>
#include <string>
#include <kafe/internal/node.hpp>
#include <iostream>
#include <optional>

namespace kafe
{
    using MaybeNodePtr = std::optional<internal::NodePtr>;

    class Parser : private internal::ParserCombinators
    {
    public:
        Parser(const std::string& code);
        ~Parser();

        void parse();
        void ASTtoString(std::ostream& os);
    
    private:
        internal::Program m_program;

        inline bool isKeyword(const std::string& name)
        {
            return (name == "fun" || name == "end" ||
                    name == "cst" || name == "cls" ||
                    name == "new" || name == "while" ||
                    name == "do"  || name == "else" ||
                    name == "if"  || name == "elif" ||
                    name == "then" || name == "true" ||
                    name == "false" || name == "ret");
        }

        inline bool isOperator(const std::string& name)
        {
            return (name == "+" || name == "-" ||
                    name == "*" || name == "/" ||
                    name == "<<" || name == "<<" ||
                    name == "~" || name == "and" ||
                    name == "or" || name == "not" ||
                    name == "==" || name == "!=" ||
                    name == "<" || name == ">" ||
                    name == "<=" || name == ">=");
        }

        bool operator_(std::string* s=nullptr);

        // parsers
        MaybeNodePtr parseInstruction();
        MaybeNodePtr parseDeclaration();
        MaybeNodePtr parseConstDef();
        MaybeNodePtr parseExp();
            MaybeNodePtr parseOperation();
            MaybeNodePtr parseSingleExp();
                MaybeNodePtr parseOperationBlock();
                MaybeNodePtr parseInt();
                MaybeNodePtr parseFloat();
                MaybeNodePtr parseString();
                MaybeNodePtr parseBool();
                MaybeNodePtr parseClassInstanciation();
                MaybeNodePtr parseFunctionCall();
                MaybeNodePtr parseMethodCall();
                MaybeNodePtr parseVarUse();
        MaybeNodePtr parseEnd();
        MaybeNodePtr parseFunction();
        MaybeNodePtr parseClass();
            MaybeNodePtr parseConstructor();
    };
}

#endif