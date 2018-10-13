#include "pch.h"

#include <vector>

#include "../Compiler_Lib/AST.h"
#include "../Compiler_Lib/parser.h"

using namespace Compiler;


TEST(ParserTest, Expressions) {
	// 1 operand
	Parser myParser = Parser("1+2");
	double exp = 3;
	std::unique_ptr<AST> res = myParser.parse();

	// I don't know how to test this yet

}