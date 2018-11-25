#pragma once
#ifndef __VISITOR_H
#define __VISITOR_H

#include "token.h"
#include "AST.h"

namespace Compiler {
	using std::unique_ptr;

	// Interface for other visitor classes to inherit from (interpreter, compiler)
	
	class Visitor {
	public:
		virtual void visit(unique_ptr<BlockAST> node) = 0;
		virtual void visit(unique_ptr<NumberAST> node) = 0;
		virtual void visit(unique_ptr<NameAST> node) = 0;
		virtual void visit(unique_ptr<ArrayAST> node) = 0;
		virtual void visit(unique_ptr<AssignmentAST> node) = 0;
		virtual void visit(unique_ptr<FuncCallAST> node) = 0;
		virtual void visit(unique_ptr<BinaryOpAST> node) = 0;
		virtual void visit(unique_ptr<UnaryOpAST> node) = 0;
		virtual void visit(unique_ptr<TernaryOpAST> node) = 0;
		virtual void visit(unique_ptr<IfAST> node) = 0;
		virtual void visit(unique_ptr<ForAST> node) = 0;
	};
}

#endif