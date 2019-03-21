#ifndef kafe_internal_parser_hpp
#define kafe_internal_parser_hpp

#include <string>
#include <stdexcept>
#include <kafe/internal/charpred.hpp>

namespace kafe
{
    namespace internal
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
            Parser(const std::string& s);
            ~Parser();
        
        private:
            std::string m_in;
            int m_count;
            int m_row;
            int m_col;
            int m_sym;

            inline void error(const std::string& error, const std::string exp)
            {
                throw ParseError(error, m_row, m_col, exp, m_sym);
            }

            // getting next character and changing the values of count/row/col/sym
            void next();

        protected:
            // basic getters
            int getCol();
            int getRow();
            int getCount();
            bool isEOF();

            void back(std::size_t n);

            /*
                Function to use and check if a Character Predicate was able to parse
                the current symbol.
                Add the symbol to the given string (if there was one) and call next()
            */
            bool accept(const CharPred& t, std::string* s=nullptr);

            /*
                Function to use and check if a Character Predicate was able to parse
                the current Symbol.
                Add the symbol to the given string (if there was one) and call next().
                Throw a ParseError if it couldn't.
            */
            bool except(const CharPred& t, std::string* s=nullptr);

            // basic parsers
            bool space       (std::string* s=nullptr);
            bool number      (std::string* s=nullptr);
            bool signedNumber(std::string* s=nullptr);
            bool name        (std::string* s=nullptr);
        };
    }
}

#endif