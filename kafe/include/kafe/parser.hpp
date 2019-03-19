#ifndef kafe_parser_hpp
#define kafe_parser_hpp

#include <string>
#include <optional>

namespace kafe
{
    // code bidon
    class Declaration {};

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

        /*
            A parser will either return a value or not depending
            on if it was able to parse the given code or not.
            If the first(s) token(s) were parsed but it found an
            unknown token, it will log the error and return std::nullopt.

            Note to future self: should set a carry flag?
        */
        std::optional<Declaration> parseDeclaration(const std::string& code);
    };
}

#endif