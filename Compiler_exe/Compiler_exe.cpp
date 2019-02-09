#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include "../Compiler_Lib/scanner.h"
#include "../Compiler_Lib/token.h"
#include "../Compiler_Lib/parser.h"
#include "../Compiler_Lib/visualizer.h"
#include "../Compiler_Lib/codegen.h"


using namespace Compiler;

std::string getInpFile(std::string path) {
	std::ifstream str;
	str.open(path);
	std::stringstream buffer;
	buffer << str.rdbuf();
	return buffer.str();
}

int main(int argc, char *argv[])
{
    /* IF TEST:
     * BEGIN
     *    DEFINE f(a, b)
     *     IF a<b THEN
     *       9+2
     *     ELSE
     *       9-2
     *     ENDIF
     *    ENDDEF
     *  END
     * BEGIN DEFINE f(a, b) IF a<b THEN 9+2 ELSE 9-2 ENDIF ENDDEF END */

    /* FOR TEST
     * BEGIN
     *   DEFINE f()
     *     FOR i = 0, i < 4, 2 IN
     *       i + 3
     *     ENDFOR
     *   ENDDEF
     * END
     * BEGIN DEFINE f() FOR i = 0, i < 4, 2 IN i + 3 ENDFOR ENDDEF END */

    /*if (argc >= 2) {
		std::cout << "Usage: " << argv[0] << " source.file [-o out.o]" << std::endl;
		exit(EXIT_FAILURE);
	}*/

	std::string code;
	std::string outName = "out.o";

    int c;
    while((c = getopt (argc, argv, "o:")) != -1) {
    	switch (c) {
    		case 'o':
    			outName = optarg;
    			std::cout << "output file: " << outName << std::endl;
    			break;
    		default:
				std::cout << "Usage: " << argv[0] << " source.file [-o out.o]" << std::endl;
				exit(EXIT_FAILURE);
		}
    }

    for (int i = optind; i < argc; i++) {
		code = getInpFile(argv[i]);
		std::cout << "file: " << argv[i] << std::endl;
    }

	std::cout << code << std::endl;
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

	auto mod = generator.emitObjCode(outName);
	
	return 0;
}

