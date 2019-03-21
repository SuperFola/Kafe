#include <kafe/internal/parser.hpp>

using namespace kafe::internal;

// NB: does not check f.good(), it's not its role
Parser::Parser(std::istream& f) :
    m_in(f.rdbuf()), m_count(0), m_row(1), m_col(1), m_sym(m_in->sbumpc())
{}

Parser::~Parser()
{}

void Parser::next()
{
    // getting a character from the stream
    m_sym = m_in->sgetc();
    m_in->snextc();

    ++m_count;

    // new line
    if (m_sym == '\n')
    {
        ++m_row;
        m_col = 1;
    }
    else if (std::isprint(m_sym))
    {
        ++m_col;
    }
}

int Parser::getCol()
{
    return m_col;
}

int Parser::getRow()
{
    return m_row;
}

int Parser::getCount()
{
    return m_count;
}

bool Parser::accept(const CharPred& t, std::string* s)
{
    // return false if the predicate couldn't consume the symbol
    if (!t(m_sym))
        return false;
    // otherwise, add it to the string and go to the next symbol
    if (s != nullptr)
        s->push_back(m_sym);
    next();
    return true;
}

bool Parser::except(const CharPred& t, std::string* s)
{
    // throw an error if the predicate couldn't consume the symbol
    if (!t(m_sym))
        error("Expected", t.name);
    // otherwise, add it to the string and go to the next symbol
    if (s != nullptr)
        s->push_back(m_sym);
    next();
    return true;
}

bool Parser::space(std::string* s)
{
    if (accept(IsSpace))
    {
        if (s != nullptr)
            s->push_back(' ');
        // loop while there are still ' ' to consume
        while (accept(IsSpace));
        return true;
    }
    return false;
}

bool Parser::number(std::string* s)
{
    if (accept(IsDigit, s))
    {
        // consume all the digits available,
        // stop when the symbole isn't a digit anymore
        while (accept(IsDigit, s));
        return true;
    }
    return false;
}

bool Parser::signedNumber(std::string* s)
{
    return accept(IsMinus, s), number(s);
}

bool Parser::name(std::string* s)
{
    // first character of a name must be alphabetic
    if (accept(IsAlpha, s))
    {
        // the next ones can be alphanumeric
        while (accept(IsAlnum, s));
        return true;
    }
    return false;
}