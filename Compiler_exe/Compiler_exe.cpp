#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include "../Compiler_Lib/scanner.h"
#include "../Compiler_Lib/token.h"
#include "../Compiler_Lib/parser.h"
#include "../Compiler_Lib/visualizer.h"
#include "../Compiler_Lib/codegen.h"


using namespace Compiler;

// Structure to hold command line arguments
struct Config {
    std::string code;
    std::string outName = "out.o";
};

// Read input file
std::string getInpFile(const std::string &path) {
	std::ifstream str;
	str.open(path);
	if (str.good()) {
        std::stringstream buffer;
        buffer << str.rdbuf();
        return buffer.str();
	} else {
        std::cout << "Unable to open file '" << path << "'" << std::endl;
        exit(EXIT_FAILURE);
	}
}

// This is awful
int linkSTL(Config config) {
    // First write stl source
    std::string stlSrc = "#include <stdio.h>\n"
                         "extern double putchard(double X){fputc((char)X,stderr);return 0;}\n"
                         "extern double printd(double X){fprintf(stderr,\"%f\\n\",X);return 0;}";
    std::ofstream outfile("stl.c");
    outfile << stlSrc << std::endl;
    outfile.close();

    std::string cmd = "cc " + config.outName + " stl.c -no-pie";
    int res = system(cmd.c_str());
    remove("stl.c");
    return res;
}

// Run compiler
int run(Config config) {
    // Set up grammar
    Parser myParser = Parser(config.code);

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

    // Parse program
    std::shared_ptr<AST> tree = myParser.parse();

    // Generate object code
    Codegen generator;

    tree->accept(&generator);

    generator.emitObjCode(config.outName);

    return linkSTL(config);
}

// Collect arguments and run
int main(int argc, char *argv[])
{
    Config config = Config();

    int c;
    while((c = getopt (argc, argv, "o:")) != -1) {
    	switch (c) {
    		case 'o':
    			config.outName = optarg;
    			std::cout << "output file: " << config.outName << std::endl;
    			break;
    		default:
				std::cout << "Usage: " << argv[0] << " source.simple [-o out.o]" << std::endl;
				exit(EXIT_FAILURE);
		}
    }

    for (int i = optind; i < argc; i++) {
		config.code = getInpFile(argv[i]);
		std::cout << "input file: " << argv[i] << std::endl;
    }

    if (config.code.empty()) {
        std::cout << "Usage: " << argv[0] << " source.simple [-o out.o]" << std::endl;
        exit(EXIT_FAILURE);
    }

    int res = run(config);
	
	return res;
}

