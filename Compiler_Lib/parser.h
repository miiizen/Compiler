#pragma once
#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include "token.h"
#include "scanner.h"
#include "AST.h"

namespace Compiler {
	class Parser;

	// Map of operator precedences
	enum Precedence {
		
		ASSIGNMENT = 1,	// =
		TERNARY,		// ? :
		LOGICAL,		// || && (right associative)
		RELATIONAL,		// == >= <= !=
		SUM,			// + -
		PRODUCT,		// * /
		EXPONENT,		// ^
		PREFIX,			// - ! (
		POSTFIX,		// ++ --
		CALL,			// ( [
	};

	/*		PARSER MODULES		*/

	// Interface for a prefix operator
	class IPrefixParser {
	public:
		virtual ~IPrefixParser() = default;

		virtual std::unique_ptr<AST> parse(Parser* parser, const Token& tok) = 0;
	};

	// Class for numbers
	class NumberParser : public IPrefixParser {
	public:
		std::unique_ptr<AST> parse(Parser* parser, const Token& tok) override;
	};

	// Class for variables
	class NameParser : public IPrefixParser {
	public:
		std::unique_ptr<AST> parse(Parser* parser, const Token& tok) override;
	};

	// Class for prefix operators
	class PrefixOperatorParser : public IPrefixParser {
	public:
		PrefixOperatorParser(Precedence prec)
			: opPrec(prec) {}
		std::unique_ptr<AST> parse(Parser* parser, const Token& tok) override;
		Precedence opPrec;
	};

	// Grouping for math expressions
	class GroupParser : public IPrefixParser {
	public:
		GroupParser(Precedence prec)
			: opPrec(prec) {}
		std::unique_ptr<AST> parse(Parser* parser, const Token& tok) override;
		Precedence opPrec;
	};

	// Interface for infix operator
	class IInfixParser {
	public:
		virtual ~IInfixParser() = default;

		virtual std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, const Token& tok) = 0;
		// Virtual method to return children's data
		virtual Precedence getPrec() = 0;

	};

	// Binary op parser
	class BinaryOperatorParser : public IInfixParser {
	public:
		BinaryOperatorParser(Precedence prec, bool right)
			: opPrec(prec), isRight(right) {}
		std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, const Token& tok) override;
		Precedence getPrec() override { return opPrec; };
		Precedence opPrec;
		bool isRight;
	};

	// Postfix op parser
	class PostfixOperatorParser : public IInfixParser {
	public:
		PostfixOperatorParser(Precedence prec)
			: opPrec(prec) {}
		std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, const Token& tok) override;
		Precedence getPrec() override { return opPrec; };
		Precedence opPrec;
	};

	// Ternary operator parser
	class TernaryOperatorParser : public IInfixParser {
	public:
		TernaryOperatorParser(Precedence prec)
			: opPrec(prec) {}
		std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, const Token& tok) override;
		Precedence getPrec() override { return opPrec; };
		Precedence opPrec;
	};

	// Assignment expressions
	class AssignmentParser : public IInfixParser {
	public:
		AssignmentParser(Precedence prec)
			: opPrec(prec) {}
		std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, const Token& tok) override;
		Precedence getPrec() override { return opPrec; };
		Precedence opPrec;
	};

	class CallParser : public IInfixParser {
	public:
		CallParser(Precedence prec)
			: opPrec(prec) {}
		std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, const Token& tok) override;
		Precedence getPrec() override { return opPrec; };
		Precedence opPrec;
	};

	// Array indexing
	class IndexParser : public IInfixParser {
	public:
		IndexParser(Precedence prec)
			: opPrec(prec) {}
		std::unique_ptr<AST> parse(Parser* parser, std::unique_ptr<AST> left, const Token& tok) override;
		Precedence getPrec() override { return opPrec; };
		Precedence opPrec;
	};

	


	/*		PARSER MAIN		*/
	// Takes series of tokens and attempts to parse them
	class Parser {
	public:
		// Constructor
		Parser(std::string inp)
			: _inp{std::move( inp )},				// Get input
			_scanner{ Scanner(_inp) }	// Initialize scanner
		{ }

		// Register prefix tokens for use in the parser
		void registerPrefixTok(TokenType tok, std::unique_ptr<IPrefixParser> parseModule);

		// Register infix tokens for use in the parser
		void registerInfixTok(TokenType tok, std::unique_ptr<IInfixParser> parseModule);

		// Register prefix unary operator
		void prefix(TokenType tok, Precedence prec);

		// Register a postfix unary operator
		void postfix(TokenType tok, Precedence prec);

		// Register left assoc binary op
		void infixLeft(TokenType tok, Precedence prec);

		// Register right assoc binary op
		void infixRight(TokenType tok, Precedence prec);

		// match a token
		bool match(TokenType tok);

		// Expect a token
		Token expect(TokenType tok);

		// Start recursive descent parsing
		std::unique_ptr<AST> parse();

		// Parse a program
		std::unique_ptr<AST> program();

		// Parse a statement block
		std::unique_ptr<AST> block();

		// Parse an if statement
		std::unique_ptr<AST> ifStmt();

		// Parse a for statement
		std::unique_ptr<AST> forStmt();

		// Math expression - TDOP
		std::unique_ptr<AST> parseExpression(int precedence = 0);

		// Return an error
		// TODO(James): hide this.  make parselets friends?
		void error(std::string message);

	private:
		// Input string
		std::string _inp;
		// Instance of a scanner that will return tokens
		Scanner _scanner;
		// Get precedence of operator
		int getPrecedence();
		// Map of prefix parser chunks
		std::map <TokenType, std::shared_ptr<IPrefixParser>> prefixMap = {};
		// Map of infix parser chunks
		std::map <TokenType, std::shared_ptr<IInfixParser>> infixMap = {};
	};
}  // namespace Compiler
#endif
