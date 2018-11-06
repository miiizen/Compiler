#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include "parser.h"
#include "token.h"
#include "AST.h"

namespace Compiler {
	/*		PARSER MODULES		*/

	/*		Numbers		*/
	std::unique_ptr<AST> NumberParser::parse(Parser * parser, const Token & tok)
	{
		// Return NumberAST with the value of the token
		std::unique_ptr<NumberAST> number = std::make_unique<NumberAST>(std::stod(tok.getValue()));
		return number;
	}

	/*		Name		*/
	std::unique_ptr<AST> NameParser::parse(Parser* parser, const Token& tok)
	{
		// Return variableAST node with the value of the token (variable name)
		std::unique_ptr<NameAST> name = std::make_unique<NameAST>(tok.getValue());
		return name;
	}

	/*		PrefixOperator		*/
	std::unique_ptr<AST> PrefixOperatorParser::parse(Parser* parser, const Token& tok)
	{
		// Parse operand
		std::unique_ptr<AST> operand = parser->parseExpression(this->opPrec);
		// Return prefix.unary op AST node
		std::unique_ptr<PrefixOpAST> op = std::make_unique<PrefixOpAST>(tok.getType(), std::move(operand));
		return op;
	}

	/*		Group Parser		*/
	std::unique_ptr<AST> GroupParser::parse(Parser * parser, const Token & tok)
	{
		std::unique_ptr<AST> expr = parser->parseExpression(0);
		parser->expect(RIGHTPAREN);
		return expr;
	}

	/*		Binary operator		*/
	std::unique_ptr<AST> BinaryOperatorParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		// Get rhs of expression.  Handle right associative things like ^ by allowing lower precedence when parsing rhs
		std::unique_ptr<AST> right = parser->parseExpression(this->opPrec - (isRight ? 1 : 0));
		// make binop AST node
		std::unique_ptr<BinaryOpAST> expr = std::make_unique<BinaryOpAST>(tok.getType(), std::move(left), std::move(right));
		return expr;
	}

	/*		Postfix operator	*/
	std::unique_ptr<AST> PostfixOperatorParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		// Wrap operand in expression
		std::unique_ptr<PostfixOpAST> expr = std::make_unique<PostfixOpAST>(tok.getType(), std::move(left));
		return expr;
	}

	/*		Ternary operator	*/
	std::unique_ptr<AST> TernaryOperatorParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		std::unique_ptr<AST> thenArm = parser->parseExpression(0);
		// match ':'
		parser->expect(COLON);
		std::unique_ptr<AST> elseArm = parser->parseExpression(Precedence::TERNARY - 1);

		// left ? thenArm : elseArm
		std::unique_ptr<TernaryOpAST> expr = std::make_unique<TernaryOpAST>(std::move(left), std::move(thenArm), std::move(elseArm));

		return expr;

	}

	/*		Assignment operator		*/
	// TODO NEEDS FINISHING NOT FINISHED FINISH ME PLEASE!!!
	std::unique_ptr<AST> AssignmentParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		// Get rhs of expression
		std::unique_ptr<AST> right = parser->parseExpression(Precedence::ASSIGNMENT - 1);

		// Check if lhs is of type name
		NameAST* var = dynamic_cast<NameAST*>(left.get());
		if (var == nullptr) {
			parser->error("The left hand side of an assignment must be a name.");
		}

		std::unique_ptr<AssignmentAST> expr = std::make_unique<AssignmentAST>(std::move(left), std::move(right));

		return expr;
	}

	/*		Function call		*/
	std::unique_ptr<AST> CallParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		// Parse comma separated values until )
		std::vector<std::shared_ptr<AST>> args = {};

		if (!parser->match(RIGHTPAREN)) {
			do {
				args.push_back(parser->parseExpression(0));
			} while (parser->match(COMMA));
			parser->expect(RIGHTPAREN);
		}

		return std::make_unique<FuncCallAST>(std::move(left), std::move(args));
	}

	/*		Access array index		*/
	std::unique_ptr<AST> Compiler::IndexParser::parse(Parser * parser, std::unique_ptr<AST> left, const Token & tok)
	{
		std::unique_ptr<AST> right = parser->parseExpression(Precedence::CALL - 1);

		// Check if lhs is of type name
		NameAST* lhs = dynamic_cast<NameAST*>(left.get());
		if (lhs == nullptr) {
			parser->error("The left hand side must be a name.");
		}

		// Check if rhs is of type number
		NumberAST* rhs = dynamic_cast<NumberAST*>(right.get());
		if (rhs == nullptr) {
			parser->error("The left hand side must be a number.");
		}

		//TODO FINISH ME PLEASE
		return nullptr;
	}


	/*		PARSER MAIN		*/

	// Register prefix token
	void Parser::registerPrefixTok(TokenType tok, std::unique_ptr<IPrefixParser> parseModule)
	{
		// Add operator value to the map
		prefixMap.insert(std::make_pair(tok, std::move(parseModule)));
	}

	// Register infix token
	void Parser::registerInfixTok(TokenType tok, std::unique_ptr<IInfixParser> parseModule)
	{
		infixMap.insert(std::make_pair(tok, std::move(parseModule)));
	}

	// Registers a prefix operator for the token and precedence
	void Parser::prefix(TokenType tok, Precedence prec)
	{
		std::unique_ptr<PrefixOperatorParser> pre = std::make_unique<PrefixOperatorParser>(prec);
		registerPrefixTok(tok, std::move(pre));
	}

	// Registers a postfix operator for the token and precedence
	void Parser::postfix(TokenType tok, Precedence prec)
	{
		std::unique_ptr<PostfixOperatorParser> post = std::make_unique<PostfixOperatorParser>(prec);
		registerInfixTok(tok, std::move(post));
	}

	// Registers a left associative binary operator for the token and precedence
	void Parser::infixLeft(TokenType tok, Precedence prec)
	{
		std::unique_ptr<BinaryOperatorParser> in = std::make_unique<BinaryOperatorParser>(prec, false);
		registerInfixTok(tok, std::move(in));
	}

	// Registers a right associative operator for the token and precedence
	void Parser::infixRight(TokenType tok, Precedence prec)
	{
		std::unique_ptr<BinaryOperatorParser> in = std::make_unique<BinaryOperatorParser>(prec, true);
		registerInfixTok(tok, std::move(in));
	}

	bool Parser::match(TokenType tok)
	{
		Token look = _scanner.lookAhead(0);
		if (look.getType() != tok) {
			return false;
		}
		_scanner.consume();
		return true;
	}

	// Expect token + consume
	Token Parser::expect(TokenType tok)
	{
		Token look = _scanner.lookAhead(0);
		if (look.getType() != tok) {
			error("Expected " + tok);
		}

		return _scanner.consume();
	}

	// Parse an expression
	std::unique_ptr<AST> Parser::parseExpression(int precedence)
	{
		Token tok = _scanner.consume();
		// Get prefix parselet

		std::shared_ptr<IPrefixParser> prefix;

		if (prefixMap.count(tok.getType()) == 1) {
			prefix = prefixMap.at(tok.getType());
		}
		else {
			throw std::exception("Unrecognised token");
		}

		// Get expression tree for the prefix
		std::unique_ptr<AST> left = prefix->parse(this, tok);

		// Get next token and see if we have an infix expression to parse
		std::shared_ptr<IInfixParser> infix;
		while (precedence < getPrecedence()) {
			tok = _scanner.consume();

			if (infixMap.count(tok.getType()) == 1) {
				infix = infixMap.at(tok.getType());
				left = infix->parse(this, std::move(left), tok);
			}

		}
		return left;
	}

	std::unique_ptr<AST> Parser::parse()
	{
		return program();
	}

	std::unique_ptr<AST> Parser::program()
	{
		// Parse an entire program
		
		expect(BEGIN);
		std::unique_ptr<AST> tree = block();
		expect(END);

		return tree;
	}

	std::unique_ptr<AST> Parser::block()
	{
		std::vector<std::shared_ptr<AST>> stmts = {};

		while ((_scanner.lookAhead(0).getType() != END) && (_scanner.lookAhead(0).getType() != ELSE) && (_scanner.lookAhead(0).getType() != ENDIF) && (_scanner.lookAhead(0).getType() != ENDFOR)) {
			Token tok = _scanner.getCurrentToken();
			switch (tok.getType()) {
			case IF:
				stmts.push_back(ifStmt());
				break;
			case FOR:
				stmts.push_back(forStmt());
				break;
			default:
				stmts.push_back(parseExpression(0));
			}
		}

		std::unique_ptr<AST> blk = std::make_unique<BlockAST>(std::move(stmts));
		return blk;
	}

	std::unique_ptr<AST> Parser::ifStmt()
	{
		// expect IF
		expect(IF);

		// get conditional expression
		std::unique_ptr<AST> cond = parseExpression(0);
		if (!cond) {
			error("A conditional expression is required.");
		}
		expect(THEN);

		// parse block
		// decl else for ahead
		std::unique_ptr<AST> elseBlock;
		std::unique_ptr<AST> thenBlock = block();

		// Check for else
		if (_scanner.lookAhead(0).getType() == ELSE) {
			expect(ELSE);
			// parse block
			elseBlock = block();
		}

		match(ENDIF);

		std::unique_ptr<IfAST> ifst = std::make_unique<IfAST>(std::move(cond), std::move(thenBlock), std::move(elseBlock));

		return ifst;
	}

	std::unique_ptr<AST> Parser::forStmt()
	{
		// expect for
		expect(FOR);

		// Get identifier
		std::string ident = expect(IDENTIFIER).getValue();

		// Expect = 
		expect(ASSIGN);

		// Parse expression
		// TODO error checking perhaps?
		std::unique_ptr<AST> start = parseExpression(0);
		if (!start) {
			error("There should be a start expression");
		}

		// expect comma
		expect(COMMA);

		// Parse expression for end value
		std::unique_ptr<AST> end = parseExpression(0);
		if (!end) {
			error("There should be an end expression");
		}

		// Optional step value.
		std::unique_ptr<AST> step;
		if (_scanner.lookAhead(0).getType() == COMMA) {
			expect(COMMA);
			step = parseExpression(0);
			// TODO error checking perhaps?
			if (!step) {
				error("There should be a step!");
			}
		}

		// Expect in
		expect(IN);

		std::unique_ptr<AST> body = block();
		if (!body) {
			error("There should be a body!");
		}

		expect(ENDFOR);

		return std::make_unique<ForAST>(ident, std::move(start), std::move(end), std::move(step), std::move(body));
	}

	// Get the precedence for a given token
	int Parser::getPrecedence()
	{
		Token tok = _scanner.lookAhead(0);
		if (infixMap.count(tok.getType()) == 1) {
			std::shared_ptr<IInfixParser> op = infixMap.at(tok.getType());
			Precedence prec = op->getPrec();
			return prec;
		}
		else {
			return 0;
		}
	}

	void Parser::error(std::string message)
	{
		throw message;
		std::cerr << "Scanner: " << message << std::endl;
	}

}
