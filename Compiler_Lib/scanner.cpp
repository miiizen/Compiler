#include "stdafx.h"
#include <iostream>
#include <string>
#include "token.h"
#include "scanner.h"

namespace Compiler {
	Token Scanner::getCurrentToken() const
	{
		return tokQueue.front();
	}

	// Return the next token from input stream
	Token Scanner::getNextToken() {
		Token curTok;
		nextChar();

		// skip comment
		if (lookChar == '#') {
			// Skip until EOL
			do {
				nextChar();
			} while (lookChar != '\n');
			nextChar();
		}

		// Eat whitespace
		if (isspace(lookChar)) {
			nextChar();
		}

		// Numbers
		if (isdigit(lookChar)) {
			std::string numStr{ getNum() };
			tokQueue.push_back(Token{ NUMBER, numStr });
		}
		// Identifiers
		else if (isalpha(lookChar)) {
			std::string identStr{ getName() };

			// Handle keywords
			if (identStr == "IF") {
				tokQueue.push_back(Token{ IF, identStr });
			}
			else if (identStr == "THEN") {
				tokQueue.push_back(Token{ THEN, identStr });
			}
			else if (identStr == "ENDIF") {
				tokQueue.push_back(Token{ ENDIF, identStr });
			}
			else if (identStr == "ELSE") {
				tokQueue.push_back(Token{ ELSE, identStr });
			}

			else if (identStr == "FOR") {
				tokQueue.push_back(Token{ FOR, identStr });
			}
			else if (identStr == "IN") {
				tokQueue.push_back(Token{ IN, identStr });
			}
			else if (identStr == "ENDFOR") {
				tokQueue.push_back(Token{ ENDFOR, identStr });
			}

			// Handle bools
			else if (identStr == "true" || identStr == "false") {
				tokQueue.push_back(Token{ BOOL, identStr });
			}

			else if (identStr == "BEGIN") {
				tokQueue.push_back(Token{ BEGIN, identStr });
			}
			
			else if (identStr == "END") {
				tokQueue.push_back(Token{ END, identStr });
			}

			// just an identifier
			else {
				tokQueue.push_back(Token{ IDENTIFIER, identStr });
			}
		}

		// Operators
		else if (isOp(lookChar)) {
			std::string opStr = getOp();
			// numeric binary operators
			if (opStr == "+") {
				tokQueue.push_back(Token{ PLUS, "+" });
			}
			else if (opStr == "-") {
				tokQueue.push_back(Token{ MINUS, "-" });
			}
			else if (opStr == "*") {
				tokQueue.push_back(Token{ STAR, "*" });
			}
			else if (opStr == "/") {
				tokQueue.push_back(Token{ SLASH, "/" });
			}
			else if (opStr == "%") {
				tokQueue.push_back(Token{ MOD, "%" });
			}
			else if (opStr == "^") {
				tokQueue.push_back(Token{ HAT, "^" });
			} 
			else if (opStr == "++") {
				tokQueue.push_back(Token{ INC, "++" });
			}
			else if (opStr == "--") {
				tokQueue.push_back(Token{ DEC, "--" });
			}

			// Comparison binary operators
			else if (opStr == "==") {
				tokQueue.push_back(Token{ EQ, "==" });
			}
			else if (opStr == "<") {
				tokQueue.push_back(Token{ LESS, "<" });
			}
			else if (opStr == ">") {
				tokQueue.push_back(Token{ GREATER, ">" });
			}
			else if (opStr == "<=") {
				tokQueue.push_back(Token{ LEQ, "<=" });
			}
			else if (opStr == ">=") {
				tokQueue.push_back(Token{ GREQ, ">=" });
			}
			else if (opStr == "!=") {
				tokQueue.push_back(Token{ NEQ, "!=" });
			}

			// Logical
			else if (opStr == "&&") {
				tokQueue.push_back(Token{ AND, "&&" });
			}
			else if (opStr == "||") {
				tokQueue.push_back(Token{ OR, "||" });
			}
			else if (opStr == "!") {
				tokQueue.push_back(Token{ NOT, "!" });
			}

			else if (opStr == "=") {
				tokQueue.push_back(Token{ ASSIGN, "=" });
			}
			else {
				error("Invalid operator '" + opStr + "'");
			}
		}

		// Parentheses
		else if (lookChar == '(') {
			tokQueue.push_back(Token{ LEFTPAREN, "(" });
		}

		else if (lookChar == ')') {
			tokQueue.push_back(Token{ RIGHTPAREN, ")" });
		}

		else if (lookChar == ',') {
			tokQueue.push_back(Token{ COMMA, "," });
		}

		// String literals
		else if (lookChar == '"') {
			std::string strLit = getString();
			tokQueue.push_back(Token{ STRING, strLit });
		}

		else if (lookChar == '?') {
			tokQueue.push_back(Token{ CONDITIONAL, "?" });
		}

		else if (lookChar == ':') {
			tokQueue.push_back(Token{ COLON, ":" });
		}

		// End of input
		else if (lookChar == ';') {
			tokQueue.push_back(Token{ END, ");" });
		}

		// Otherwise
		else {
			error("Unexpected " + std::string(1, lookChar) + " in input");
		}

		// Return value just added to the queue
		return tokQueue.back();
	}

	Token Scanner::consume() {
		// Read token
		lookAhead(0);

		Token ret = tokQueue.front();
		tokQueue.pop_front();

		return ret;
	}

	// Get and return the next n tokens
	Token Scanner::lookAhead(int distance) {
		for (int i = -1; i++; i > distance) {
			getNextToken();
		}
		return tokQueue.at(distance);
	}

	/* Methods */
	char Scanner::nextChar() {
		try {
			// TODO BAD BAD BAD EOL REALLY NEEDS SORTING OUT
			if (pos > _inp.length() - 1) {
				lookChar = ';';
				// Infinite loops rip
				//error("nextChar: Reached end of input without terminator.");
			}
			else {
				lookChar = _inp.at(pos);
				pos++;
			}
			return lookChar;
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
				// This was causing infinite loops.
				// A missing ; is not a massive error at the moment as it's recoverable
				//error("peekChar: Reached end of input without terminator.");
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

	bool Scanner::isOp(const char &op) const {
		// Switch all characters which may start an operand
		switch (op) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '^':
		case '%':
		case '=':
		case '>':
		case '<':
		case '!':
		case '|':
		case '&':
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

	// <number> ::= [<digit>]+.[<digit>]+
	std::string Scanner::getNum() {
		// SHOULD CHECK FOR STARTING DIGIT BEFORE CALL TO ME
		std::string numStr(1, lookChar);
		while (isdigit(peekChar())) {
			numStr += nextChar();
		}

		// deal with decimal point
		if (peekChar() == '.') {
			numStr += nextChar();
			// get remaining number after decimal point
			while (isdigit(peekChar())) {
				numStr += nextChar();
			}
		}

		// make sure only legal separators come after the number
		char peek{ peekChar() };
		if (!isWhite(peek) && peek != ')' && peek != ',' && peek != ';' && !isOp(peek)) {
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
		while (isOp(peekChar())) {
			opStr += nextChar();
		}
		skipWhite();
		return opStr;
	}
}