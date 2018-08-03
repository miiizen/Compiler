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
		// Get a whole token from input stream
		Token getToken();

	private:
		// Input stream
		std::string _inp;
		// Current position
		int pos = 0;
		// The lookahead character read
		char lookChar;
		// Current token
		Token _token;

		/* Methods */
		// Return next character, increase pos
		char nextChar();
		// Peek next character without increasing pos
		char peekChar();
		// Expect a character
		void expect(char ch);
		// Return an error
		void error(std::string message);

		/* Recognisers */
		// Recognise a binary operator
		bool isBinOp(char op);
		// Recognise a unary operator
		bool isUnOp(char op);
		// Recognise whitespace
		bool isWhite(char op);

		/* Consumers */
		void skipWhite();
		// Get a keyword
		std::string getName();
		// Get a number
		std::string getNum();
		// Get an operator
		std::string getOp();

	};
}

#endif  // __SCANNER_H
