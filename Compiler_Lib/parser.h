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

	// To quickly check associativity
	static enum Associativity { RIGHT, LEFT };

	// struct to hold info on precedence and associativity of operators
	static struct OpInfo {
		OpInfo(int _prec, Associativity _assoc)
			: precedence(_prec), assoc(_assoc) 
		{ }

		int precedence;
		Associativity assoc;
	};

	// Map containing info on operators
	static std::map <std::string, OpInfo> opInfoMap {
		{ "+", OpInfo(1, LEFT) },
		{ "-", OpInfo(1, LEFT) },
		{ "*", OpInfo(2, LEFT) },
		{ "/", OpInfo(2, LEFT) },
		{ "^", OpInfo(3, RIGHT) }
	};

	// Takes series of tokens and attempts to parse them
	class Parser {
	public:
		// Constructor
		Parser(std::string inp)
			: _inp{ inp },				// Get input
			_scanner{ Scanner(_inp) }	// Initialize scanner
		{ }

		// Start recursive descent parsing
		std::unique_ptr<AST> parse();

	private:
		// Input string
		std::string _inp;
		// Instance of a scanner that will return tokens
		Scanner _scanner;

		/* Methods */
		// Return an error
		void error(std::string message);

		/* Parsing */
		// PRECEDENCE CLIMBING
		// Deal with sub expression TODO Should this return Token?
		std::unique_ptr<AST> parseAtom();
		// Math expression - precedence climbing
		std::unique_ptr<AST> parseExpression(int minPrec);
		// Deal with operator
		std::unique_ptr<AST> parseOp(const std::string& opType, std::unique_ptr<AST> subExpLhs, std::unique_ptr<AST> subExpRhs);
	};
}
#endif