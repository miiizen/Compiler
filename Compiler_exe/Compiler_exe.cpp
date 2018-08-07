// Compiler_exe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Compiler_Lib/scanner.h"
#include "../Compiler_Lib/token.h"
#include "../Compiler_Lib/parser.h"

using namespace Compiler;

int main()
{
	Parser myParser = Parser("(1+2)*3;");
	myParser.parse();

	return 0;
}
