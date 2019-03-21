#ifndef kafe_parser_hpp
#define kafe_parser_hpp

#include <kafe/internal/parser.hpp>
#include <string>

namespace kafe
{
    class Parser : private internal::Parser
    {
    public:
        Parser(const std::string& code);
        ~Parser();

        void parse();
    
    private:
        // parsers
        bool parseDeclaration();
        bool parseExp();
    };
}

#endif