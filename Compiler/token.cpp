#include "stdafx.h"
#include <iostream>
#include "token.h"

// Overload << operator for token class.  Take references to output stream and token class as arguments
std::ostream &operator<<(std::ostream &strm, const Token &token) {
	return strm << "Token(" << token._tokType << ", " << token._value << ")";
}

TokenType Token::getType()
{
	return _tokType;
}

std::string Token::getValue()
{
	return _value;
}
