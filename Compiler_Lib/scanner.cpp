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
		if (isspace(lookChar) != 0) {
			nextChar();
		}

		// Numbers
		if (isdigit(lookChar) != 0) {
			std::string numStr{ getNum() };
			tokQueue.emplace_back( NUMBER, numStr );
		}
		// Identifiers
		else if (isalpha(lookChar) != 0) {
			std::string identStr{ getName() };

			// Handle keywords
			if (identStr == "IF") {
				tokQueue.emplace_back( IF, identStr );
			}
			else if (identStr == "THEN") {
				tokQueue.emplace_back( THEN, identStr );
			}
			else if (identStr == "ENDIF") {
				tokQueue.emplace_back( ENDIF, identStr );
			}
			else if (identStr == "ELSE") {
				tokQueue.emplace_back( ELSE, identStr );
			}

			else if (identStr == "FOR") {
				tokQueue.emplace_back( FOR, identStr );
			}
			else if (identStr == "IN") {
				tokQueue.emplace_back( IN, identStr );
			}
			else if (identStr == "ENDFOR") {
				tokQueue.emplace_back( ENDFOR, identStr );
			}

			// Handle bools
			else if (identStr == "true" || identStr == "false") {
				tokQueue.emplace_back( BOOL, identStr );
			}

			else if (identStr == "BEGIN") {
				tokQueue.emplace_back( BEGIN, identStr );
			}
			
			else if (identStr == "END") {
				tokQueue.emplace_back( END, identStr );
			}

			else if (identStr == "DEFINE") {
				tokQueue.emplace_back( DEFINE, identStr );
			}

			// just an identifier
			else {
				tokQueue.emplace_back( IDENTIFIER, identStr );
			}
		}

		// Operators
		else if (isOp(lookChar)) {
			std::string opStr = getOp();
			// numeric binary operators
			if (opStr == "+") {
				tokQueue.emplace_back( PLUS, "+" );
			}
			else if (opStr == "-") {
				tokQueue.emplace_back( MINUS, "-" );
			}
			else if (opStr == "*") {
				tokQueue.emplace_back( STAR, "*" );
			}
			else if (opStr == "/") {
				tokQueue.emplace_back( SLASH, "/" );
			}
			else if (opStr == "%") {
				tokQueue.emplace_back( MOD, "%" );
			}
			else if (opStr == "^") {
				tokQueue.emplace_back( HAT, "^" );
			} 
			else if (opStr == "++") {
				tokQueue.emplace_back( INC, "++" );
			}
			else if (opStr == "--") {
				tokQueue.emplace_back( DEC, "--" );
			}

			// Comparison binary operators
			else if (opStr == "==") {
				tokQueue.emplace_back( EQ, "==" );
			}
			else if (opStr == "<") {
				tokQueue.emplace_back( LESS, "<" );
			}
			else if (opStr == ">") {
				tokQueue.emplace_back( GREATER, ">" );
			}
			else if (opStr == "<=") {
				tokQueue.emplace_back( LEQ, "<=" );
			}
			else if (opStr == ">=") {
				tokQueue.emplace_back( GREQ, ">=" );
			}
			else if (opStr == "!=") {
				tokQueue.emplace_back( NEQ, "!=" );
			}

			// Logical
			else if (opStr == "&&") {
				tokQueue.emplace_back( AND, "&&" );
			}
			else if (opStr == "||") {
				tokQueue.emplace_back( OR, "||" );
			}
			else if (opStr == "!") {
				tokQueue.emplace_back( NOT, "!" );
			}

			else if (opStr == "=") {
				tokQueue.emplace_back( ASSIGN, "=" );
			}
			else {
				error("Invalid operator '" + opStr + "'");
			}
		}

		// Parentheses
		else if (lookChar == '(') {
			tokQueue.emplace_back( LEFTPAREN, "(" );
		}

		else if (lookChar == ')') {
			tokQueue.emplace_back( RIGHTPAREN, ")" );
		}

		else if (lookChar == ',') {
			tokQueue.emplace_back( COMMA, "," );
		}

		// String literals
		else if (lookChar == '"') {
			std::string strLit = getString();
			tokQueue.emplace_back( STRING, strLit );
		}

		else if (lookChar == '?') {
			tokQueue.emplace_back( CONDITIONAL, "?" );
		}

		else if (lookChar == ':') {
			tokQueue.emplace_back( COLON, ":" );
		}

		// End of input
		else if (lookChar == ';') {
			tokQueue.emplace_back( END, ");" );
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
		for (int i = -1; i < distance; i++) {
			getNextToken();
		}
		return tokQueue.at(distance);
	}

	/* Methods */
	char Scanner::nextChar() {
		try {
			// TODO(James): BAD BAD BAD EOL REALLY NEEDS SORTING OUT
			if (pos > (_inp.length() - 1)) {
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
		catch (std::runtime_error& e) {
			error("nextChar: " + std::string(e.what()));
		}

		return 0;
	}

	char Scanner::peekChar() {
		try {
			if (pos > _inp.length() - 1) {
				// TODO(James): BAD BAD BAD THIS FEELS WRONG
				// If we have reached the end of the input without encountering a semi colon return one anyway?
				return ';';
				// This was causing infinite loops.
				// A missing ; is not a massive error at the moment as it's recoverable
				//error("peekChar: Reached end of input without terminator.");
			}
			
				return _inp.at(pos);
			
		}
		catch (std::runtime_error& e) {
			error("peekChar: " + std::string(e.what()));
		}

		return 0;
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
		return op == ' ' || op == '\n' || op == '\t';
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
		while (isalnum(peekChar()) != 0) {
			identStr += nextChar();
		}
		skipWhite();
		return identStr;
	}

	// <number> ::= [<digit>]+.[<digit>]+
	std::string Scanner::getNum() {
		// SHOULD CHECK FOR STARTING DIGIT BEFORE CALL TO ME
		std::string numStr(1, lookChar);
		while (isdigit(peekChar()) != 0) {
			numStr += nextChar();
		}

		// deal with decimal point
		if (peekChar() == '.') {
			numStr += nextChar();
			// get remaining number after decimal point
			while (isdigit(peekChar()) != 0) {
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
}  // namespace Compiler
