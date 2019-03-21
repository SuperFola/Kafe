#ifndef kafe_parser_hpp
#define kafe_parser_hpp

#include <string>
#include <optional>
#include <stdexcept>
#include <kafe/internal/charpred.hpp>

namespace kafe
{
    struct ParseError : public std::runtime_error
    {
        const int row;
        const int col;
        const std::string exp;
        const int sym;

        ParseError(const std::string& what, int row, int col, std::string exp, int sym) :
            std::runtime_error(what), row(row), col(col), exp(std::move(exp)), sym(sym)
        {}
    };

    class Parser
    {
    public:
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