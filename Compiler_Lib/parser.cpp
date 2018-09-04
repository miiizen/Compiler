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
		std::unique_ptr<AST> op = std::make_unique<PrefixOpAST>(tok.getValue(), std::move(operand));
		return op;
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

		try {
			prefix = prefixMap.at(tok.getType());
		}
		catch(std::exception &e) {
			throw std::exception("Could not parse token");
		}

		return prefix->parse(this, tok);
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
