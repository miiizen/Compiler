#include <iostream>
#include "token.h"
#include "AST.h"
#include <memory>

namespace Compiler {
	using std::unique_ptr;


	/*		Accept methods for visitor which can't be inlined.		*/	
	void BlockAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void NumberAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void NameAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void ArrayAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void AssignmentAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void FuncCallAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void FuncDefAST::accept(Visitor *v)
	{
		v->visit(this);
	}

	void ReturnAST::accept(Visitor *v)
	{
		v->visit(this);
	}

	void BinaryOpAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void UnaryOpAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void TernaryOpAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void IfAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

	void ForAST::accept(Visitor *v)
	{		
		v->visit(this);
	}

}  // namespace Compiler
