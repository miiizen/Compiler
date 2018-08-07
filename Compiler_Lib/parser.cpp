#include "stdafx.h"
#include "parser.h"
#include "token.h"
#include <iostream>
#include <string>

//// TODO work out which of the getNextTokens should be getCurrentToken.

namespace Compiler {
	void Parser::parse()
	{
		parseExpression(1);
	}
	void Parser::error(std::string message)
	{
		throw message;
		std::cerr << "Scanner: " << message << std::endl;
	}

	// Deal with sub expression
	std::unique_ptr<AST> Parser::parseAtom() {
		// Get the current token
		Token curTok = _scanner.getCurrentToken();

		// Found a left parenthesis, start of sub expression.
		if (curTok.getType() == LEFTPAREN) {
			std::unique_ptr<AST> val = parseExpression(1);
			curTok = _scanner.getNextToken();
			// Closing ) should follow sub expression
			if (curTok.getType() != RIGHTPAREN) {
				error("Unmatched '('");
			}
			return val;
		}
		// If source ends... This should be picked up in the scanner I think?
		else if (curTok.getType() == END) {
			error("Source ended unexpectedly");
		}
		// Got an operator not a subexpression
		else if (curTok.getType() == BINOP) {
			error("Expected an expression, not an operator");
		}
		// Deal with a single number
		// Create AST node
		else if (curTok.getType() == NUMBER) {
			double val = std::stod(curTok.getValue());
			NumberAST num = NumberAST(val);
			std::unique_ptr<AST> unum = std::make_unique<AST>(num);
			return unum;
		}
		// TODO deal with a variable/identifier
		else if (curTok.getType() == IDENTIFIER) {
			VariableAST var = VariableAST(curTok.getValue());
			std::unique_ptr<AST> uvar = std::make_unique<AST>(var);
			return uvar;
		}
	}

	std::unique_ptr<AST> Parser::parseExpression(int minPrec)
	{
		std::unique_ptr<AST> subExpLhs = parseAtom();

		while (true) {
			Token curTok = _scanner.getCurrentToken();

			// We have a number for the lhs of the expression, or precedence is less than minimum
			if (curTok.getType() == END || curTok.getType() == RIGHTPAREN || curTok.getType() != BINOP || opInfoMap.at(curTok.getValue()).precedence < minPrec) {
				break;
			}

			// From here, current token should be a binary operator
			// TODO test for binop?

			// Get prec + assoc and calculate min prec for recursive call
			std::string op = curTok.getValue();
			int prec = opInfoMap.at(op).precedence;
			Associativity assoc = opInfoMap.at(op).assoc;
			// If assoc is left, prec + 1, else prec
			int nextMinPrec = (assoc == LEFT) ? prec + 1 : prec;

			// ???
			_scanner.getNextToken();

			std::unique_ptr<AST> subExpRhs = parseExpression(nextMinPrec);

			// Update lhs with new value
			// TODO we need to work out what we actually want to do here, implementation details please
			subExpLhs = parseOp(op, std::move(subExpLhs), std::move(subExpRhs));
		}

		return subExpLhs;
	}
	std::unique_ptr<AST> Parser::parseOp(std::string opType, std::unique_ptr<AST> subExpLhs, std::unique_ptr<AST> subExpRhs)
	{
		std::unique_ptr<BinaryOpAST> binOp = std::make_unique<BinaryOpAST>(opType, std::move(subExpLhs), std::move(subExpRhs));
		return binOp;
	}
}
