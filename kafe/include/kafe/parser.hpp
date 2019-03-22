#ifndef kafe_parser_hpp
#define kafe_parser_hpp

#include <kafe/internal/parser.hpp>
#include <string>
#include <kafe/internal/node.hpp>
#include <iostream>

namespace kafe
{
    class Parser : private internal::ParserCombinators
    {
    public:
        Parser(const std::string& code);
        ~Parser();

        void parse();
        void ASTtoString(std::ostream& os);
    
    private:
        internal::Program m_program;
        internal::NodePtr m_node;

        // parsers
        bool parseDeclaration();
        bool parseConstDef();
        bool parseExp();
            bool parseInt();
            bool parseFloat();
            bool parseString();
            bool parseBool();
    };
}

#endif