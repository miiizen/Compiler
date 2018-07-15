// Compiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>

int main()
{
	Scanner scan{ "if 128716328*40 endif else;" };
	Token tok{};
	do {
		tok = scan.getToken();
		std::cout << tok << std::endl;
	} while (tok.getType() != END);

	return 0;
}
