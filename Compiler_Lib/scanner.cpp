#include "stdafx.h"
#include <iostream>
#include <string>
#include "token.h"
#include "scanner.h"

namespace Compiler {
	// Return the next token from input stream
	Token Scanner::getToken() {
		Token curTok;
		nextChar();

		// Eat whitespace
		if (isspace(lookChar)) {
			nextChar();
		}

		// Numbers
		if (isdigit(lookChar)) {
			std::string numStr{ getNum() };
			return Token{ NUMBER, numStr };
		}
		// Identifiers
		else if (isalpha(lookChar)) {
			std::string identStr{ getName() };

			// Handle keywords
			if (identStr == "if") {
				return Token{ IF, identStr };
			}
			else if (identStr == "endif") {
				return Token{ ENDIF, identStr };
			}

			else if (identStr == "else") {
				return Token{ ELSE, identStr };
			}

			// just an identifier
			else {
				return Token{ IDENTIFIER, identStr };
			}
		}

		// Operators
		else if (isBinOp(lookChar)) {
			std::string lookStr{ lookChar };
			return Token{ BINOP, lookStr };
		}

		/*else if (isUnOp(lookChar)) {
			std::string lookStr{ lookChar };
			return Token{ UNOP, lookStr };
		}*/

		// Parentheses
		else if (lookChar == '(') {
			return Token{ LEFTPAREN, "(" };
		}

		else if (lookChar == ')') {
			return Token{ RIGHTPAREN, ")" };
		}

		// End of input
		else if (lookChar == ';') {
			return Token{ END, ";" };
		}

		// Otherwise
		else {
			error("Unexpected " + std::string(1, lookChar) + " in input");
		}
	}

	/* Methods */
	char Scanner::nextChar() {
		try {
			lookChar = _inp.at(pos);
			pos++;
		}
		catch (std::exception& e) {
			error("nextChar: " + std::string(e.what()));
		}

		return lookChar;
	}

	char Scanner::peekChar() {
		try {
			return _inp.at(pos);
		}
		catch (std::exception& e) {
			error("peekChar: " + std::string(e.what()));
		}
	}

	void Scanner::expect(char ch) {
		if (peekChar() != ch) {
			error("Expected '" + std::string(1, ch) + "'.");
		}
		else {
			nextChar();
		}
	}

	void Scanner::error(std::string message) {
		throw message;
		std::cerr << message << std::endl;
	}

	/* Recognisers */

	bool Scanner::isBinOp(char op) {
		switch (op) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '^':
			return true;
		default:
			return false;
		}
	}

	bool Scanner::isUnOp(char op) {
		switch (op) {
		case '-':
			return true;
		default:
			return false;
		}
	}

	bool Scanner::isWhite(char op)
	{
		if (op == ' ' || op == '\n' || op == '\t') {
			return true;
		}
		return false;
	}

	/* Consumers */

	void Scanner::skipWhite() {
		char look{ peekChar() };
		if (isWhite(look)) {
			// Space found - eat it
			nextChar();
		}
	}

	// identifier ::= [a-zA-Z][a-zA-Z0-9]*
	std::string Scanner::getName() {
		// SHOULD CHECK FOR [a-zA-Z] BEFORE CALL TO ME
		std::string identStr(1, lookChar);
		while (isalnum(peekChar())) {
			identStr += nextChar();
		}
		skipWhite();
		return identStr;
	}

	// <number> ::= [<digit>]+
	std::string Scanner::getNum() {
		// SHOULD CHECK FOR STARTING DIGIT BEFORE CALL TO ME
		std::string numStr(1, lookChar);
		while (isdigit(peekChar())) {
			numStr += nextChar();
		}

		// make sure only legal separators come after the number
		char peek{ peekChar() };
		if (!isWhite(peek) && peek != ')' && peek != ';' && !isBinOp(peek) && !isUnOp(peek)) {
			error("Unexpected " + std::string(1, peek) + " in digit");
		}
		skipWhite();
		return numStr;
	}


	std::string Scanner::getOp()
	{
		// SHOULD CHECK FOR FIRST CHAR BEFORE CALL TO ME
		std::string opStr(1, lookChar);
		while (isBinOp(peekChar()) || isUnOp(peekChar())) {
			opStr += nextChar();
		}
		skipWhite();
		return opStr;
	}

}