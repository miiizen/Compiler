#include "stdafx.h"
#include "token.h"
#include "scanner.h"
#include <iostream>
#include <string>

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

			// Handle bools
			else if (identStr == "true" || identStr == "false") {
				return Token{ BOOL, identStr };
			}

			// just an identifier
			else {
				return Token{ IDENTIFIER, identStr };
			}
		}

		// Operators
		else if (isBinOp(lookChar)) {
			return { Token(BINOP, std::string(1, lookChar)) };
			//switch (lookChar) {
			//case '+':
			//	return{ Token(PLUS, "+") };
			//case '-':
			//	return{ Token(MINUS, "-") };
			//case '*':
			//	return{ Token(MULTIPLY, "*") };
			//case '/':
			//	return{ Token(DIVIDE, "/") };
			//case '^':
			//	return{ Token(POW, "^") };
			//}
		}

		// Parentheses
		else if (lookChar == '(') {
			return Token{ LEFTPAREN, "(" };
		}

		else if (lookChar == ')') {
			return Token{ RIGHTPAREN, ")" };
		}

		// String literals
		else if (lookChar == '"') {
			std::string strLit = getString();
			return Token{ STRING, strLit };
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
			// TODO BAD BAD BAD EOL REALLY NEEDS SORTING OUT
			if (pos > _inp.length() - 1) {
				return ';';
				error("nextChar: Reached end of input without terminator.");
			}
			else {
				lookChar = _inp.at(pos);
				pos++;
				return lookChar;
			}
		}
		catch (std::exception& e) {
			error("nextChar: " + std::string(e.what()));
		}
	}

	char Scanner::peekChar() {
		try {
			if (pos > _inp.length() - 1) {
				// TODO BAD BAD BAD THIS FEELS WRONG
				// If we have reached the end of the input without encountering a semi colon return one anyway?
				return ';';
				error("peekChar: Reached end of input without terminator.");
			}
			else {
				return _inp.at(pos);
			}
		}
		catch (std::exception& e) {
			error("peekChar: " + std::string(e.what()));
		}
	}

	// Verify char is expected then eat.
	void Scanner::expect(const char &ch) {
		if (lookChar != ch) {
			error("Expected '" + std::string(1, ch) + "'.");
		}
		else {
			nextChar();
		}
	}

	void Scanner::error(std::string message) {
		std::cerr << "Scanner: " << message << std::endl;
		// Recover from error by skipping token and trying to resume
		// Not sure how useful this really is, but we'll see
		nextChar();
	}

	/* Recognisers */

	bool Scanner::isBinOp(const char &op) const {
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

	bool Scanner::isUnOp(const char &op) const {
		switch (op) {
		case '-':
			return true;
		default:
			return false;
		}
	}

	bool Scanner::isWhite(const char &op) const {
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

	// <string> ::= " [\w*] "
	std::string Scanner::getString() {
		// Eat opening "
		expect('"');
		std::string litString;

		while (peekChar() != '"') {
			litString += nextChar();
		}
		// Eat closing "
		expect('"');

		return litString;
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