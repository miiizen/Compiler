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

	TokenType Token::getType() const
	{
		return _tokType;
	}

	std::string Token::getValue() const
	{
		return _value;
	}
}
