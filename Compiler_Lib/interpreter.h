#pragma once
#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include "AST.h"

namespace Compiler {
	using std::unique_ptr;

	// 'execute' AST as we traverse each node
	class Interpreter : public Visitor {

	public:
		// TODO(James): We want to pass values up and down the nodes as we visit them.  State should be STORED in the visitor
		void visit(unique_ptr<BlockAST> node) override;
		void visit(unique_ptr<NumberAST> node) override;
		void visit(unique_ptr<NameAST> node) override;
		void visit(unique_ptr<ArrayAST> node) override;
		void visit(unique_ptr<AssignmentAST> node) override;
		void visit(unique_ptr<FuncCallAST> node) override;
		void visit(unique_ptr<BinaryOpAST> node) override;
		void visit(unique_ptr<UnaryOpAST> node) override;
		void visit(unique_ptr<TernaryOpAST> node) override;
		void visit(unique_ptr<IfAST> node) override;
		void visit(unique_ptr<ForAST> node) override;
	};
}  // namespace Compiler
#endif
