// Compiler_exe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Compiler_Lib/scanner.h"
#include "../Compiler_Lib/token.h"

using namespace Compiler;
int main()
{
	Scanner myScan = Scanner("if");
	Token exp = Token(IF, "if");
	Token result = myScan.getToken();
    return 0;
}

