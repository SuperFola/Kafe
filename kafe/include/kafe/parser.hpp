#ifndef kafe_parser_hpp
#define kafe_parser_hpp

#include <string>
#include <optional>
#include <kafe/internal/charpred.hpp>

namespace kafe
{
    class Parser
    {
    public:
        // default constructor for Parser, initializing code to "", doing nothing else
        Parser();
        ~Parser();

        // adding some code to m_code, not analyzing it yet
        void feed(const std::string& code);
        // parse code, checking for errors
        void parse();
    
    private:
        std::string m_code;

        bool accept(const CharPred& t, std::string& s);
    };
}

#endif