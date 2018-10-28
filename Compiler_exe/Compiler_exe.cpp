// Compiler_exe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Compiler_Lib/scanner.h"
#include "../Compiler_Lib/token.h"
#include "../Compiler_Lib/parser.h"


using namespace Compiler;

int main()
{
	Parser myParser = Parser("a(1, 2, b)");

	// Set up grammar
	myParser.registerPrefixTok(IDENTIFIER, std::make_unique<NameParser>());
	myParser.registerPrefixTok(NUMBER, std::make_unique<NumberParser>());
	myParser.registerPrefixTok(LEFTPAREN, std::make_unique<GroupParser>(PREFIX));
	myParser.registerInfixTok(LEFTPAREN, std::make_unique<CallParser>(CALL));

	myParser.prefix(MINUS, PREFIX);
	myParser.prefix(PLUS, PREFIX);

	myParser.postfix(INC, POSTFIX);
	myParser.postfix(DEC, POSTFIX);

	myParser.infixLeft(PLUS, SUM);
	myParser.infixLeft(MINUS, SUM);
	myParser.infixLeft(STAR, PRODUCT);
	myParser.infixLeft(SLASH, PRODUCT);
	myParser.infixRight(HAT, EXPONENT);

	std::unique_ptr<AST> par = myParser.parse();

	return 0;
}
