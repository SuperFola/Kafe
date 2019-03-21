#include <kafe/parser.hpp>

using namespace kafe;

Parser::Parser(const std::string& code) :
    internal::Parser(std::istringstream(code))
{}

Parser::Parser(std::istream& input) :
    internal::Parser(input)
{}

Parser::~Parser()
{}

void Parser::parse()
{}