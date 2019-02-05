#include <memory>
#include <codegen.h>
#include "../Compiler_Lib/scanner.h"
#include "../Compiler_Lib/token.h"
#include "../Compiler_Lib/parser.h"
#include "../Compiler_Lib/visualizer.h"
#include "../Compiler_Lib/codegen.h"


using namespace Compiler;

int main()
{
    std::string code = "BEGIN DEFINE f(a, b) 7+7 ENDDEF END";
	std::cout << code << std::endl << std::endl;
	Parser myParser = Parser(code);

	// Set up grammar

	// Names, numbers, assign
	myParser.registerPrefixTok(IDENTIFIER, std::make_unique<NameParser>());
	myParser.registerPrefixTok(NUMBER, std::make_unique<NumberParser>());
	myParser.registerInfixTok(ASSIGN, std::make_unique<AssignmentParser>(ASSIGNMENT));
	myParser.registerPrefixTok(DEFINE, std::make_unique<FunctionParser>());

	// Parens
	myParser.registerPrefixTok(LEFTPAREN, std::make_unique<GroupParser>(PREFIX));
	myParser.registerInfixTok(LEFTPAREN, std::make_unique<CallParser>(CALL));

	// a ? b : c
	myParser.registerInfixTok(CONDITIONAL, std::make_unique<TernaryOperatorParser>(TERNARY));

	// prefix +, -, !
	myParser.prefix(MINUS, PREFIX);
	myParser.prefix(PLUS, PREFIX);
	myParser.prefix(NOT, PREFIX);

	// ++, --
	myParser.postfix(INC, POSTFIX);
	myParser.postfix(DEC, POSTFIX);

	// Infix arithmetic
	myParser.infixLeft(PLUS, SUM);
	myParser.infixLeft(MINUS, SUM);
	myParser.infixLeft(STAR, PRODUCT);
	myParser.infixLeft(SLASH, PRODUCT);
	myParser.infixLeft(MOD, PRODUCT);
	myParser.infixRight(HAT, EXPONENT);

	// logical ops
	myParser.infixRight(AND, LOGICAL);
	myParser.infixRight(OR, LOGICAL);

	// relational ops
	myParser.infixLeft(EQ, RELATIONAL);
	myParser.infixLeft(NEQ, RELATIONAL);
	myParser.infixLeft(LESS, RELATIONAL);
	myParser.infixLeft(GREATER, RELATIONAL);
	myParser.infixLeft(LEQ, RELATIONAL);
	myParser.infixLeft(GREQ, RELATIONAL);

	std::shared_ptr<AST> tree = myParser.parse();

	Codegen generator;

	tree->accept(&generator);


    std::cout << "made it lol" << std::endl;
	
	return 0;
}
