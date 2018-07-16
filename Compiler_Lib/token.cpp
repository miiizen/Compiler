#include "stdafx.h"
#include <iostream>
#include "token.h"

namespace Compiler {
	// Overload << operator for token class.  Take references to output stream and token class as arguments
	std::ostream &operator<<(std::ostream &strm, const Token &token) {
		return strm << "Token(" << token._tokType << ", " << token._value << ")";
	}

	bool operator==(const Token & lhs, const Token & rhs)
	{
		if (lhs._tokType == rhs._tokType && rhs._value == rhs._value) {
			return true;
		}
		else { return false; }
	}

	const TokenType Token::getType()
	{
		return _tokType;
	}

	const std::string Token::getValue()
	{
		return _value;
	}
}