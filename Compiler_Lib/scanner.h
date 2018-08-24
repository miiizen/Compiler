#pragma once
#ifndef __SCANNER_H
#define __SCANNER_H

#include <iostream>
#include <string>
#include "token.h"

namespace Compiler {
	// Splits input into stream of tokens
	class Scanner {
	public:
		// Constructor
		Scanner(std::string inp)
			: _inp{ inp }
		{ }
		// Return the current token
		Token getCurrentToken() const;
		// Get the next token from input stream
		Token getNextToken();

	private:
		// Input stream
		std::string _inp;
		// Current position
		int pos = 0;
		// The lookahead character read
		char lookChar;
		// Current token
		Token currentToken;

		/* Methods */
		// Return next character, increase pos
		char nextChar();
		// Peek next character without increasing pos
		char peekChar();
		// Expect a character
		void expect(const char &ch);
		// Return an error
		void error(std::string message);

		/* Recognisers */
		// Recognise an operator
		bool isOp(const char &op) const;
		// Recognise whitespace
		bool isWhite(const char &op) const;

		/* Consumers */
		void skipWhite();
		// Get a keyword
		std::string getName();
		// Get a number
		std::string getNum();
		// Get a string literal
		std::string getString();
		// Get an operator
		std::string getOp();

	};
}

#endif  // __SCANNER_H
