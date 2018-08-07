#include "pch.h"

#include <vector>

#include "../Compiler_Lib/scanner.h"
#include "../Compiler_Lib/token.h"

using namespace Compiler;

TEST(ScannerTest, HandlesInt) {
	// SCAN SINGLE DIGIT
	Scanner myScan = Scanner("1;");
	Token exp = Token(NUMBER, "1");
	Token result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to read single digit integer";

	// SCAN MULTIPLE DIGITS
	myScan = Scanner("1234;");
	exp = Token(NUMBER, "1234");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to read multi digit integer";

	// FAIL ON UNEXPECED CHAR
	myScan = Scanner("12r8");
	exp = Token(NUMBER, "128");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result);
	//ASSERT_THROW(myScan.getNextToken(), std::string) << "Failed to throw error on integer containing an alpha character";
}

TEST(ScannerTest, HandlesKeywords) {
	Scanner myScan = Scanner("if;");
	Token exp = Token(IF, "if");
	Token result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to identify 'if'";

	myScan = Scanner("else;");
	exp = Token(ELSE, "else");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to identify 'else'";

	myScan = Scanner("endif;");
	exp = Token(ENDIF, "endif");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to identify 'endif'";
}

TEST(ScannerTest, HandlesIdentifiers) {
	Scanner myScan = Scanner("ident;");
	Token exp = Token(IDENTIFIER, "ident");
	Token result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get alpha identifier";

	myScan = Scanner("ident23;");
	exp = Token(IDENTIFIER, "ident23");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get alphanumeric identifier";
}

TEST(ScannerTest, HandlesOperators) {
	Scanner myScan = Scanner("+;");
	Token exp = Token(BINOP, "+");
	Token result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get binary '+' operator";

	myScan = Scanner("-;");
	exp = Token(BINOP, "-");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get binary '-' operator";

	myScan = Scanner("*;");
	exp = Token(BINOP, "*");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get binary '*' operator";

	myScan = Scanner("/;");
	exp = Token(BINOP, "/");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get binary '/' operator";

	myScan = Scanner("^;");
	exp = Token(BINOP, "^");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get binary '^' operator";

	myScan = Scanner("-;");
	exp = Token(BINOP, "-");
	result = myScan.getNextToken();

	ASSERT_EQ(exp, result) << "Failed to get binary '-' operator";
}

TEST(ScannerTest, HandlesExpressions) {
	// BASIC EXPRESSION
	{
		Scanner myScan = Scanner("12+3;");
		Token result;
		std::vector<Token> results;
		std::vector<Token> exp{ Token(NUMBER, "12"), Token(BINOP, "+"), Token(NUMBER, "3"), Token(END, ";") };
		do {
			result = myScan.getNextToken();
			results.push_back(result);
		} while (result.getType() != END);

		ASSERT_EQ(results, exp) << "Expression scanned is not expected";
	}

	// WHITESPACE
	{
		Scanner myScan = Scanner("12 + 3;");
		Token result;
		std::vector<Token> results;
		std::vector<Token> exp{ Token(NUMBER, "12"), Token(BINOP, "+"), Token(NUMBER, "3"), Token(END, ";") };
		do {
			result = myScan.getNextToken();
			results.push_back(result);
		} while (result.getType() != END);

		ASSERT_EQ(results, exp) << "Expression with whitespace was not expected";
	}

	// PARENTHESES
	{
		Scanner myScan = Scanner("(12 + 3) * 5;");
		Token result;
		std::vector<Token> results;
		std::vector<Token> exp{ Token(LEFTPAREN, "("), Token(NUMBER, "12"), Token(BINOP, "+"), Token(NUMBER, "3"),Token(RIGHTPAREN, ")"), Token(BINOP, "*"), Token(NUMBER, "5"), Token(END, ";") };
		do {
			result = myScan.getNextToken();
			results.push_back(result);
		} while (result.getType() != END);

		ASSERT_EQ(results, exp) << "Expression with parentheses was not expected";
	}
}

TEST(ScannerTest, HandlesStrings) {
	Scanner myScan = Scanner("\"test string\"");
	Token exp = Token(STRING, "test string");
	Token result = myScan.getNextToken();

	ASSERT_EQ(result, exp) << "Failed to get string literal";

	//TODO test for failure to parse string, missing " etc.
}

TEST(ScannerTest, HandlesBools) {
	// Get true
	Scanner myScan = Scanner("true");
	Token exp = Token(BOOL, "true");
	Token result = myScan.getNextToken();

	ASSERT_EQ(result, exp) << "Failed to get bool 'true'";


	// Get false
	myScan = Scanner("false");
	exp = Token(BOOL, "false");
	result = myScan.getNextToken();

	ASSERT_EQ(result, exp) << "Failed to get bool 'false'";
}

TEST(ScannerTest, HandlesComments) {
	Scanner myScan = Scanner("#comment\n2");
	Token exp = Token(NUMBER, "2");
	Token result = myScan.getNextToken();

	ASSERT_EQ(result, exp) << "Failed to read single number after comment";

	// Token before and after comment
	myScan = Scanner("1 #comment comment\n2");
	std::vector<Token> expToks{ Token(NUMBER, "1"), Token(NUMBER, "2"), Token(END, ";") };
	std::vector<Token> results;

	do {
		result = myScan.getNextToken();
		results.push_back(result);
	} while (result.getType() != END);

	ASSERT_EQ(results, expToks) << "Failed to read number before and after comment";

}