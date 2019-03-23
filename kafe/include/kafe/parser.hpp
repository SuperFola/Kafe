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

        // parsers
        MaybeNodePtr parseInstruction();
        MaybeNodePtr parseDeclaration();
        MaybeNodePtr parseConstDef();
        MaybeNodePtr parseExp();
            MaybeNodePtr parseInt();
            MaybeNodePtr parseFloat();
            MaybeNodePtr parseString();
            MaybeNodePtr parseBool();
            MaybeNodePtr parseFunctionCall();
        MaybeNodePtr parseEnd();
        MaybeNodePtr parseFunction();
    };
}

#endif