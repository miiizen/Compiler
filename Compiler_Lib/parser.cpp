#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "parser.h"
#include "token.h"
#include "AST.h"


namespace Compiler {
	using std::unique_ptr;

	/*		PARSER MODULES		*/

	/*		Numbers		*/
	unique_ptr<AST> NumberParser::parse(Parser * parser, const Token & tok)
	{
		// Return NumberAST with the value of the token
		unique_ptr<NumberAST> number = std::make_unique<NumberAST>(std::stod(tok.getValue()));
		return number;
	}

	/*		Name		*/
	unique_ptr<AST> NameParser::parse(Parser* parser, const Token& tok)
	{
		// Return variableAST node with the value of the token (variable name)
		unique_ptr<NameAST> name = std::make_unique<NameAST>(tok.getValue());
		return name;
	}

	/*		Function definition		*/
	std::unique_ptr<AST> FunctionParser::parse(Parser *parser, const Token &tok)
	{
		// DEFINE [EXT] f(a, b, c)
		//    ...
		// ENDDEF
		// Already consumed DEFINE

		// Check to see if function is extern
		bool ext = parser->match(EXT);

		// get name
		unique_ptr<AST> name = parser->parseExpression(DEFINITON);
		// Check name is a name
		if (name->getType() != ASTType::NAME) {
			parser->error("Function name must be of type name.");
		}

		parser->expect(LEFTPAREN);

		// Parse comma separated values until )
		std::vector<std::shared_ptr<AST>> args = {};

		if (!parser->match(RIGHTPAREN)) {
			do {
				args.push_back(parser->parseExpression());
			} while (parser->match(COMMA));
			parser->expect(RIGHTPAREN);
		}

		// Expect a function body if the definition is not external
		// Body is null for an extern
		unique_ptr<AST> body(nullptr);

		if (!ext) {
			body = parser->block();
			if (!body) {
				parser->error("Function definition expects a body!");
			}
			parser->expect(ENDDEF);
		}
		// For an external definition, there is no block to close, so no ENDDEF

		return std::make_unique<FuncDefAST>(std::move(name), ext, std::move(args), std::move(body));
	}

	/*		PrefixOperator		*/
	unique_ptr<AST> PrefixOperatorParser::parse(Parser* parser, const Token& tok)
	{
		// Parse operand
		unique_ptr<AST> operand = parser->parseExpression(this->opPrec);
		// Return prefix.unary op AST node
		unique_ptr<UnaryOpAST> op = std::make_unique<UnaryOpAST>(tok.getType(), std::move(operand));
		return op;
	}

	/*		Group Parser		*/
	unique_ptr<AST> GroupParser::parse(Parser * parser, const Token & tok)
	{
		unique_ptr<AST> expr = parser->parseExpression(ASSIGNMENT);
		parser->expect(RIGHTPAREN);
		return expr;
	}

	/*		Binary operator		*/
	unique_ptr<AST> BinaryOperatorParser::parse(Parser * parser, unique_ptr<AST> left, const Token & tok)
	{
		// Get rhs of expression.  Handle right associative things like ^ by allowing lower precedence when parsing rhs
		unique_ptr<AST> right = parser->parseExpression(this->opPrec - (isRight ? 1 : 0));
		// make binop AST node
		unique_ptr<BinaryOpAST> expr = std::make_unique<BinaryOpAST>(tok.getType(), std::move(left), std::move(right));
		return expr;
	}

	/*		Postfix operator	*/
	unique_ptr<AST> PostfixOperatorParser::parse(Parser * parser, unique_ptr<AST> left, const Token & tok)
	{
		// Wrap operand in expression
		unique_ptr<UnaryOpAST> expr = std::make_unique<UnaryOpAST>(tok.getType(), std::move(left));
		return expr;
	}

	/*		Ternary operator	*/
	unique_ptr<AST> TernaryOperatorParser::parse(Parser * parser, unique_ptr<AST> left, const Token & tok)
	{
		unique_ptr<AST> thenArm = parser->parseExpression();
		// match ':'
		parser->expect(COLON);
		unique_ptr<AST> elseArm = parser->parseExpression(Precedence::TERNARY - 1);

		// left ? thenArm : elseArm
		unique_ptr<TernaryOpAST> expr = std::make_unique<TernaryOpAST>(std::move(left), std::move(thenArm), std::move(elseArm));

		return expr;

	}

	/*		Assignment operator		*/
	unique_ptr<AST> AssignmentParser::parse(Parser * parser, unique_ptr<AST> left, const Token & tok)
	{
		// Get rhs of expression
		unique_ptr<AST> right = parser->parseExpression(Precedence::ASSIGNMENT - 1);

		// Check if lhs is of type name
		if (left->getType() != ASTType::NAME) {
			parser->error("The left hand side of an assignment must be a name.");
		}

		unique_ptr<AssignmentAST> expr = std::make_unique<AssignmentAST>(std::move(left), std::move(right));

		return expr;
	}

	/*		Function call		*/
	unique_ptr<AST> CallParser::parse(Parser * parser, unique_ptr<AST> left, const Token & tok)
	{
		// Check if lhs is a name
		if (left->getType() != ASTType::NAME) {
			parser->error("The left hand side of a function call must be a name");
		}
		// Parse comma separated values until )
		std::vector<std::shared_ptr<AST>> args = {};

		if (!parser->match(RIGHTPAREN)) {
			do {
				args.push_back(parser->parseExpression());
			} while (parser->match(COMMA));
			parser->expect(RIGHTPAREN);
		}

		return std::make_unique<FuncCallAST>(std::move(left), std::move(args));
	}

	/*		Access array index		*/
	unique_ptr<AST> Compiler::IndexParser::parse(Parser * parser, unique_ptr<AST> left, const Token & tok)
	{
		unique_ptr<AST> right = parser->parseExpression(Precedence::CALL - 1);

		// Check if lhs is of type name
		if (left->getType() == ASTType::NAME) {
			parser->error("The left hand side of an array index must be a name.");
		}

		// Check if rhs is of type number
		if (left->getType() == ASTType::NUMBER) {
			parser->error("The left hand side of an array index must be a number.");
		}

		// TODO(James): FINISH ME PLEASE
		return nullptr;
	}


	/*		PARSER MAIN		*/

	// Register prefix token
	void Parser::registerPrefixTok(TokenType tok, unique_ptr<IPrefixParser> parseModule)
	{
		// Add operator value to the map
		prefixMap.insert(std::make_pair(tok, std::move(parseModule)));
	}

	// Register infix token
	void Parser::registerInfixTok(TokenType tok, unique_ptr<IInfixParser> parseModule)
	{
		infixMap.insert(std::make_pair(tok, std::move(parseModule)));
	}

	// Registers a prefix operator for the token and precedence
	void Parser::prefix(TokenType tok, Precedence prec)
	{
		unique_ptr<PrefixOperatorParser> pre = std::make_unique<PrefixOperatorParser>(prec);
		registerPrefixTok(tok, std::move(pre));
	}

	// Registers a postfix operator for the token and precedence
	void Parser::postfix(TokenType tok, Precedence prec)
	{
		unique_ptr<PostfixOperatorParser> post = std::make_unique<PostfixOperatorParser>(prec);
		registerInfixTok(tok, std::move(post));
	}

	// Registers a left associative binary operator for the token and precedence
	void Parser::infixLeft(TokenType tok, Precedence prec)
	{
		unique_ptr<BinaryOperatorParser> in = std::make_unique<BinaryOperatorParser>(prec, false);
		registerInfixTok(tok, std::move(in));
	}

	// Registers a right associative operator for the token and precedence
	void Parser::infixRight(TokenType tok, Precedence prec)
	{
		unique_ptr<BinaryOperatorParser> in = std::make_unique<BinaryOperatorParser>(prec, true);
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
			error("Unexpected '" + look.getValue() + "'");
		}

		return _scanner.consume();
	}

	// Parse an expression
	unique_ptr<AST> Parser::parseExpression(int precedence)
	{
		Token tok = _scanner.consume();
		// Get prefix parselet

		std::shared_ptr<IPrefixParser> prefix;

		if (prefixMap.count(tok.getType()) == 1) {
			prefix = prefixMap.at(tok.getType());
		}
		else {
			error("Unrecognised token '" + tok.getValue() + "'.");
		}

		// Get expression tree for the prefix
		unique_ptr<AST> left = prefix->parse(this, tok);

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

	unique_ptr<AST> Parser::parse()
	{
		return program();
	}

	unique_ptr<AST> Parser::program()
	{
		// Parse an entire program
		
		expect(BEGIN);
		unique_ptr<AST> tree = block();
		expect(END);

		return tree;
	}

	unique_ptr<AST> Parser::block()
	{
		std::vector<std::shared_ptr<AST>> stmts = {};

		while ((_scanner.lookAhead(0).getType() != END) && (_scanner.lookAhead(0).getType() != ELSE) && (_scanner.lookAhead(0).getType() != ENDIF) && (_scanner.lookAhead(0).getType() != ENDFOR) && (_scanner.lookAhead(0).getType() != ENDDEF)) {
			Token tok = _scanner.getCurrentToken();
			switch (tok.getType()) {
			case IF:
				stmts.push_back(ifStmt());
				break;
			case FOR:
				stmts.push_back(forStmt());
				break;
			default:
				stmts.push_back(parseExpression());
			}
		}

		unique_ptr<AST> blk = std::make_unique<BlockAST>(std::move(stmts));
		return blk;
	}

	unique_ptr<AST> Parser::ifStmt()
	{
		// expect IF
		expect(IF);

		// get conditional expression
		unique_ptr<AST> cond = parseExpression();
		if (!cond) {
			error("A conditional expression is required.");
		}
		expect(THEN);

		// parse block
		// decl else for ahead
		unique_ptr<AST> elseBlock;
		unique_ptr<AST> thenBlock = block();
		if (!thenBlock) {
			error("Body of if statement is required");
		}

		// Check for else
		if (_scanner.lookAhead(0).getType() == ELSE) {
			expect(ELSE);
			// parse block
			elseBlock = block();
		}

		expect(ENDIF);

		unique_ptr<IfAST> ifst = std::make_unique<IfAST>(std::move(cond), std::move(thenBlock), std::move(elseBlock));

		return ifst;
	}

	unique_ptr<AST> Parser::forStmt()
	{
		// expect for
		expect(FOR);

		// Get identifier
		std::string ident = expect(IDENTIFIER).getValue();

		// Expect = 
		expect(ASSIGN);

		// Parse expression
		unique_ptr<AST> start = parseExpression();
		if (!start) {
			error("Expected a start expression in for loop");
		}

		// expect comma
		expect(COMMA);

		// Parse expression for end value
		unique_ptr<AST> end = parseExpression();
		if (!end) {
			error("Expected an end expression in for loop");
		}

		// Optional step value.
		unique_ptr<AST> step;
		if (_scanner.lookAhead(0).getType() == COMMA) {
			expect(COMMA);
			step = parseExpression();
			if (!step) {
				error("Expected a start expression after comma in for loop");
			}
		}

		// Expect in
		
		expect(IN);

		unique_ptr<AST> body = block();
		if (!body) {
			error("Expected a body expression in for loop");
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
		
			return 0;
	}

	void Parser::error(std::string message)
	{
		throw std::runtime_error("Parser: " + message);
	}

}  // namespace Compiler
