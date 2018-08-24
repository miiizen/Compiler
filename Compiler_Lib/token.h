#pragma once
#ifndef __TOKEN_H
#define __TOKEN_H

#include <iostream>
#include <string>

namespace Compiler {
	// Enum representing different types of token
	enum TokenType {
		// Operators
		PLUS, MINUS, STAR, SLASH, HAT,
		EQ, LESS, GREATER, LEQ, GREQ,
		AND, OR, NOT,
		ASSIGN,

		LEFTPAREN, RIGHTPAREN,
		NUMBER, STRING, IDENTIFIER, BOOL,
		IF, ENDIF, ELSE,
		NEWLINE, END
	};

	// Class representing a token, its type and value
	class Token {
	public:
		// Constructors
		Token()
		{ }
		Token(TokenType tokenType, std::string value)
			: _tokType{ tokenType }, _value{ value }
		{ }

		// Return token type
		TokenType getType() const;

		// Return token value
		std::string getValue() const;


	private:
		// The type of the token
		TokenType _tokType;
		// The value of the token
		std::string _value;

		// Overload << to output token in a pretty way
		// Declared as friend to access private members
		friend std::ostream& operator<<(std::ostream&, const Token&);

		friend bool operator==(const Token& lhs, const Token& rhs);

	};
}

#endif  // __TOKEN_H
