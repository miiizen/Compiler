#pragma once
#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include "AST.h"

namespace Compiler {
	using std::unique_ptr;

	// 'execute' AST as we traverse each node
	class Interpreter : public Visitor {

	public:
		// TODO We want to pass values up and down the nodes as we visit them.  State should be STORED in the visitor
		virtual void visit(unique_ptr<BlockAST> node);
		virtual void visit(unique_ptr<NumberAST> node);
		virtual void visit(unique_ptr<NameAST> node);
		virtual void visit(unique_ptr<ArrayAST> node);
		virtual void visit(unique_ptr<AssignmentAST> node);
		virtual void visit(unique_ptr<FuncCallAST> node);
		virtual void visit(unique_ptr<BinaryOpAST> node);
		virtual void visit(unique_ptr<UnaryOpAST> node);
		virtual void visit(unique_ptr<TernaryOpAST> node);
		virtual void visit(unique_ptr<IfAST> node);
		virtual void visit(unique_ptr<ForAST> node);
	};
}
#endif