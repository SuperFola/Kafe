#include <kafe/internal/parser.hpp>
#include <iostream>

using namespace kafe::internal;

ParserCombinators::ParserCombinators(const std::string& s) :
    m_in(s), m_count(0), m_row(1), m_col(1)
{
    // if the input string is empty, raise an error
    if (s.size() == 0)
    {
        m_sym = EOF;
        error("Expected symbol, got empty string", "");
    }

    // otherwise, get the first symbol
    next();
}

ParserCombinators::~ParserCombinators()
{}

void ParserCombinators::next()
{
    // getting a character from the stream
    m_sym = m_in[m_count];
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

int ParserCombinators::getCol()
{
    return m_col;
}

int ParserCombinators::getRow()
{
    return m_row;
}

int ParserCombinators::getCount()
{
    return m_count;
}

std::size_t ParserCombinators::getSize()
{
    return m_in.size();
}

bool ParserCombinators::isEOF()
{
    return m_sym == '\0';
}

void ParserCombinators::back(std::size_t n)
{
    // going back into the string and adjusting the rows count
    for (std::size_t i=0; i < n && m_count > 0; i++)
    {
        m_sym = m_in[m_count];
        --m_count;

        if (m_sym == '\n')
            --m_row;
    }

    // adjusting the columns count
    int count = m_count;
    while (m_in[count] != '\n' && count != 0)
        --count;
    m_col = m_count - count;

    next();  // getting the 'new' current character
}

bool ParserCombinators::accept(const CharPred& t, std::string* s)
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

bool ParserCombinators::except(const CharPred& t, std::string* s)
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

bool ParserCombinators::space(std::string* s)
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

bool ParserCombinators::number(std::string* s)
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

bool ParserCombinators::signedNumber(std::string* s)
{
    return accept(IsMinus, s), number(s);
}

bool ParserCombinators::name(std::string* s)
{
    // first character of a name must be alphabetic
    if (accept(IsAlpha, s))
    {
        // the next ones can be alphanumeric, or '_'
        while (accept(IsAlnum, s) || accept(IsChar('_'), s));
        return true;
    }
    return false;
}