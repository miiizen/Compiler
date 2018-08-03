#include "stdafx.h"
#include "parser.h"
#include "token.h"
#include <iostream>
#include <string>

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
	AST Parser::parseAtom() {
		// Get next token
		Token curTok = _scanner.getToken();

		// Found a left parenthesis, start of sub expression.
		if (curTok.getType() == LEFTPAREN) {
			curTok = _scanner.getToken();
			AST val = parseExpression(1);
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
		else if(curTok.getType() == NUMBER) {
			double val = std::stod(curTok.getValue());
			NumberAST num = NumberAST(val);
			return num;
		}
		// TODO deal with a variable/identifier
		else if (curTok.getType() == IDENTIFIER) {
			VariableAST var = VariableAST(curTok.getValue());
			return var;
		}
	}

	AST Parser::parseExpression(int minPrec)
	{
		AST subExpLhs = parseAtom();

		while (true) {
			Token curTok;
			try {
				curTok = _scanner.getToken();
			} 
			catch (std::exception &e) {
				break;
			}

			// We have a number for the lhs of the expression, or precedence is less than minimum
			if (curTok.getType() == END || curTok.getType() != BINOP || opInfoMap.at(curTok.getValue()).precedence < minPrec) {
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

			AST subExpRhs = parseExpression(nextMinPrec);

			// Update lhs with new value
			// TODO we need to work out what we actually want to do here, implementation details please
			subExpLhs = parseOp(op, subExpLhs, subExpRhs);
		}

		return subExpLhs;
	}
	AST Parser::parseOp(std::string opType, AST subExpLhs, AST subExpRhs)
	{
		// Prepare unique pointers for both sides
		/*std::unique_ptr<AST> lhs(&subExpLhs);
		std::unique_ptr<AST> rhs(&subExpRhs);*/

		BinaryOpAST binOp = BinaryOpAST(opType, subExpLhs, subExpRhs);
		return binOp;
	}
}
