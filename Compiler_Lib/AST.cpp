#include "stdafx.h"
#include <iostream>
#include "token.h"
#include "AST.h"

namespace Compiler {
	using std::unique_ptr;


	/*		Accept methods for visitor which can't be inlined.		*/	
	void BlockAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<BlockAST>(this));
	}

	void NumberAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<NumberAST>(this));
	}

	void NameAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<NameAST>(this));
	}

	void ArrayAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<ArrayAST>(this));
	}

	void AssignmentAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<AssignmentAST>(this));
	}

	void FuncCallAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<FuncCallAST>(this));
	}

	void BinaryOpAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<BinaryOpAST>(this));
	}

	void UnaryOpAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<UnaryOpAST>(this));
	}

	void TernaryOpAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<TernaryOpAST>(this));
	}

	void IfAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<IfAST>(this));
	}

	void ForAST::accept(Visitor * v)
	{
		v->visit(unique_ptr<ForAST>(this));
	}

}  // namespace Compiler
