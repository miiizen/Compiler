#pragma once
#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <string>
#include <map>
#include "token.h"
#include "scanner.h"
#include "AST.h"

namespace Compiler {
	class Parser;
	/*		PARSER MODULES		*/

	// Interface for a prefix operator
	class IPrefixParser {
	public:
		virtual ~IPrefixParser() = default;

		virtual std::unique_ptr<AST> parse(Parser* parser, const Token& tok) = 0;
	};

	// Class for variables
	class NameParser : public IPrefixParser {
	public:
		virtual std::unique_ptr<AST> parse(Parser* parser, const Token& tok);
	};

	// Class for prefix operators
	class PrefixOperatorParser : public IPrefixParser {
	public:
		virtual std::unique_ptr<AST> parse(Parser* parser, const Token& tok);
	};

	// Interface for infix operator
	class IInfix {
	public:
		virtual ~IInfix() = 0;

		virtual std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, Token tok) = 0;
		virtual int getPrecedence() = 0;
	};

	/*		OP INFO		*/
	// To quickly check associativity
	enum Associativity { RIGHT, LEFT };

	// struct to hold info on precedence and associativity of operators
	struct OpInfo {
		OpInfo(int _prec, Associativity _assoc)
			: precedence(_prec), assoc(_assoc) 
		{ }

		int precedence;
		Associativity assoc;
	};

	// Map containing info on operators
	static std::map <TokenType, OpInfo> opInfoMap {
		{ PLUS, OpInfo(1, LEFT) },
		{ MINUS, OpInfo(1, LEFT) },
		{ STAR, OpInfo(2, LEFT) },
		{ SLASH, OpInfo(2, LEFT) },
		{ HAT, OpInfo(3, RIGHT) }
	};


	/*		PARSER MAIN		*/
	// Takes series of tokens and attempts to parse them
	class Parser {
	public:
		// Constructor
		Parser(std::string inp)
			: _inp{ inp },				// Get input
			_scanner{ Scanner(_inp) }	// Initialize scanner
		{ }

		// Register prefix tokens for use in the parser
		void registerPrefixTok(TokenType tok, std::unique_ptr<IPrefixParser> parseModule);

		// Register infix tokens for use in the parser
		void registerInfixTok(TokenType tok, std::unique_ptr<IInfix> parseModule);

		// Register prefix unary operator
		void prefix(TokenType tok);

		// Start recursive descent parsing
		std::unique_ptr<AST> parse();

		// Math expression - TDOP
		std::unique_ptr<AST> parseExpression();

	private:
		// Input string
		std::string _inp;
		// Instance of a scanner that will return tokens
		Scanner _scanner;
		// Map of prefix parser chunks
		std::map <TokenType, std::shared_ptr<IPrefixParser>> prefixMap = {};

		/* Methods */
		// Return an error
		void error(std::string message);


	};
}
#endif