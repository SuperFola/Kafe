#ifndef kafe_parser_hpp
#define kafe_parser_hpp

#include <kafe/internal/parser.hpp>
#include <string>
#include <sstream>

namespace kafe
{
    class Parser : private internal::Parser
    {
        Parser(const std::string& code);
        Parser(std::istream& input);
        ~Parser();

        void parse();
    };
}

#endif