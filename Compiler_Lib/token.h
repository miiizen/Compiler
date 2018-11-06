#pragma once
#ifndef __TOKEN_H
#define __TOKEN_H

#include <iostream>
#include <string>

namespace Compiler {
	// Enum representing different types of token
	enum TokenType {
		// Operators
		PLUS, MINUS, STAR, SLASH, HAT, MOD, INC, DEC,
		EQ, LESS, GREATER, LEQ, GREQ, NEQ,
		AND, OR, NOT,
		ASSIGN, CONDITIONAL, COLON,

		LEFTPAREN, RIGHTPAREN, LEFTSQ, RIGHTSQ, COMMA,
		NUMBER, STRING, IDENTIFIER, BOOL,
		BEGIN, IF, ENDIF, ELSE, THEN,
		FOR, IN, ENDFOR,
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
