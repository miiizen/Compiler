#pragma once
#ifndef __VIS_H
#define __VIS_H

#include <memory>
#include <map>
#include "AST.h"

namespace Compiler {
	using std::unique_ptr;
	using std::shared_ptr;

	// output some code for a .DOT file on each node
	class Visualizer : public Visitor {
	private:
		//std::unique_ptr<AST> tree;
		int tabs;

		void printText(std::string text, bool end = true, bool tabs = true);

		std::map<TokenType, std::string> opMap {
			{ PLUS, "+" },
			{ MINUS, "-" },
			{ STAR, "*" },
			{ SLASH, "/" },
			{ HAT, "^" },
			{ MOD, "%" },
			{ INC, "++" },
			{ DEC, "--" },
			{ EQ, "==" },
			{ LESS, "<" },
			{ GREATER, ">" },
			{ LEQ, "<=" },
			{ GREQ, ">=" },
			{ NEQ, "!="  },
			{ AND, "&&" },
			{ OR, "||" },
			{ NOT, "!" },
			{ ASSIGN, "=" }
		};


	public:
		// traverse
		Visualizer() {}
		void preorder(AST* tree);
		void visit(BlockAST* node) override;
		void visit(NumberAST* node) override;
		void visit(NameAST* node) override;
		void visit(ArrayAST* node) override;
		void visit(AssignmentAST* node) override;
		void visit(FuncCallAST* node) override;
		void visit(BinaryOpAST* node) override;
		void visit(UnaryOpAST* node) override;
		void visit(TernaryOpAST* node) override;
		void visit(IfAST* node) override;
		void visit(ForAST* node) override;
	};
}  // namespace Compiler
#endif
