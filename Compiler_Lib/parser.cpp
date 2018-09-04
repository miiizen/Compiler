#include "stdafx.h"
#include <iostream>
#include <string>
#include "parser.h"
#include "token.h"
#include "AST.h"

namespace Compiler {
	/*		PARSER MODULES		*/

	/*		Name		*/
	std::unique_ptr<AST> NameParser::parse(Parser* parser, const Token& tok)
	{
		// Return variableAST node with the value of the token (variable name)
		std::unique_ptr<VariableAST> name = std::make_unique<VariableAST>(tok.getValue());
		return name;
	}

	/*		PrefixOperator		*/
	std::unique_ptr<AST> PrefixOperatorParser::parse(Parser* parser, const Token& tok)
	{
		// Parse operand
		std::unique_ptr<AST> operand = parser->parseExpression();
		// Return prefix.unary op AST node
		std::unique_ptr<AST> op = std::make_unique<PrefixOpAST>(tok.getType(), std::move(operand));
		return op;
	}

	/*		Binary operator		*/
	std::unique_ptr<AST> Compiler::BinaryOperatorParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		// Get rhs of expression
		std::unique_ptr<AST> right = parser->parseExpression();
		// make binop AST node
		std::unique_ptr<AST> expr = std::make_unique<BinaryOpAST>(tok.getType(), std::move(left), std::move(right));
		return expr;
	}

	/*		Postfix operator	*/
	std::unique_ptr<AST> Compiler::PostfixOperatorParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		// Wrap operand in expression
		std::unique_ptr<AST> expr = std::make_unique<PostfixOpAST>(tok.getType(), std::move(left));
		return expr;
	}

	/*		Ternary operator	*/
	std::unique_ptr<AST> Compiler::TernaryOperatorParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		std::unique_ptr<AST> thenArm = parser->parseExpression();
		// TODO match ':'
		std::unique_ptr<AST> elseArm = parser->parseExpression();

		// left ? thenArm : elseArm
		std::unique_ptr<AST> expr = std::make_unique<TernaryOpAST>(std::move(left), std::move(thenArm), std::move(elseArm));

		return expr;

	}


	/*		PARSER MAIN		*/

	void Parser::registerPrefixTok(TokenType tok, std::unique_ptr<IPrefixParser> parseModule)
	{
		// Add operator value to the map
		prefixMap.insert(std::make_pair(tok, std::move(parseModule)));
	}


	std::unique_ptr<AST> Parser::parseExpression()
	{
		Token tok = _scanner.getNextToken();
		// Get prefix parselet

		std::shared_ptr<IPrefixParser> prefix;

		if (prefixMap.count(tok.getType()) == 1) {
			prefix = prefixMap.at(tok.getType());
		}
		else {
			throw std::exception("Could not parse token");
		}

		// Get expression tree for the prefix
		std::unique_ptr<AST> left = prefix->parse(this, tok);

		// Get next token and see if we have an infix expression to parse
		tok = _scanner.getNextToken();
		std::shared_ptr<IInfixParser> infix;

		if (infixMap.count(tok.getType()) == 1) {
			infix = infixMap.at(tok.getType());
		}
		else {
			return left;
		}

		return infix->parse(this, std::move(left), tok);
	}

	void Parser::prefix(TokenType tok)
	{
		std::unique_ptr<PrefixOperatorParser> pre = std::make_unique<PrefixOperatorParser>();
		registerPrefixTok(tok, std::move(pre));
	}

	std::unique_ptr<AST> Parser::parse()
	{
		// Advance to first token
		//_scanner.getNextToken();
		return parseExpression();
	}

	void Parser::error(std::string message)
	{
		throw message;
		std::cerr << "Scanner: " << message << std::endl;
	}
}
