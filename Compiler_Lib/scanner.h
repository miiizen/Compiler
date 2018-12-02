#include <utility>

#pragma once
#ifndef __SCANNER_H
#define __SCANNER_H

#include <iostream>
#include <string>
#include <deque>
#include "token.h"

namespace Compiler {
	// Splits input into stream of tokens
	class Scanner {
	public:
		// Constructor
		Scanner(std::string inp)
			: _inp{std::move( inp )}
		{ }
		// Return the current token
		Token getCurrentToken() const;
		Token consume();
		// Get a queue of lookahead tokens.  This allows the parser to be LL(k)
		Token lookAhead(int distance);

	private:
		// Input stream
		std::string _inp;
		// Current position
		int pos = 0;
		// The lookahead character read
		char lookChar;
		// Current token
		//Token currentToken;
		// Queue of upcoming tokens
		std::deque<Token> tokQueue;


		/* Methods */
		// Get the next token from input stream
		Token getNextToken();
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
}  // namespace Compiler

#endif  // __SCANNER_H
