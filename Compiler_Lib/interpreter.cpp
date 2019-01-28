#include <memory>
#include "interpreter.h"

namespace Compiler {
	using std::unique_ptr;

	// TODO(James): implement

	void Interpreter::visit(unique_ptr<BlockAST> node)
	{
		// iterate through list of children
		//		do a visit on each child in list
		//		depth first
	}

	void Interpreter::visit(unique_ptr<NumberAST> node)
	{
		// Get val from the pointer
		double val = node->getVal();
		// wow realy useful, what are you going to do with that now?
	}

	void Interpreter::visit(unique_ptr<NameAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<ArrayAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<AssignmentAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<FuncCallAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<BinaryOpAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<UnaryOpAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<TernaryOpAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<IfAST> node)
	{
		
	}

	void Interpreter::visit(unique_ptr<ForAST> node)
	{
		
	}
}  // namespace Compiler
