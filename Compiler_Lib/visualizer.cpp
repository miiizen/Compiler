#include "visualizer.h"
#include <memory>
#include "AST.h"

namespace Compiler {
	using std::unique_ptr;
	using std::shared_ptr;

	void Visualizer::printText(std::string text, bool end, bool doTabs)
	{
		std::string prtStr;
		if (doTabs) {
			for (int i = 0; i < tabs; i++) {
				prtStr += "\t";
			}
		}
		prtStr += text;
		std::cout << prtStr;
		if (end) { std::cout << std::endl; }
	}

	// We want a preorder traversal: root, left, right
	void Visualizer::preorder(AST* tree)
	{
		tabs = 0;
		printText("PROGRAM:");
		tabs += 1;
		tree->accept(this);
	}

	void Visualizer::visit(BlockAST* node)
	{
		printText("BLOCK:");
		tabs += 1;
		for (const auto &i : node->getChildren()) {
			i->accept(this);
		}
	} 

	void Visualizer::visit(NumberAST* node)
	{
		double val = node->getVal();
		std::string v = std::to_string(val);
		printText(" " + v + " ", false, false);
	}

	void Visualizer::visit(NameAST* node)
	{
		printText(" " + node->getName() + " ", false, false);
	}

	void Visualizer::visit(ArrayAST* node)
	{
		//TODO(James)
	}

	void Visualizer::visit(AssignmentAST* node)
	{
		node->getName()->accept(this);
		printText(" = ", false, false);
		node->getRhs()->accept(this);
	}

	void Visualizer::visit(FuncCallAST* node)
	{
		//TODO(James)
	}

	void Visualizer::visit(BinaryOpAST* node)
	{
		std::string op = opMap[node->getOp()];
		node->getLhs()->accept(this);
		printText(" " + op + " ", false, false);
		node->getRhs()->accept(this);
	}

	void Visualizer::visit(UnaryOpAST* node)
	{
		std::string op = opMap[node->getOp()];
		printText(" " + op, false, false);
		node->getOperand()->accept(this);
	}

	void Visualizer::visit(TernaryOpAST* node)
	{

	}

	void Visualizer::visit(IfAST* node)
	{

	}

	void Visualizer::visit(ForAST* node)
	{
		printText("FOR:");
		printText("Start: ", false);
		auto start = node->getStart();
		start->accept(this);

		printText("  End: ", false, false);
		auto end = node->getEnd();
		end->accept(this);

		printText("  Step: ", false, false);
		auto step = node->getStep();
		step->accept(this);
		std::cout << std::endl;

		//tabs += 1;
		auto bod = node->getBody();
		bod->accept(this);
	}

	void Visualizer::visit(FuncDefAST *node)
	{

	}
}  // namespace Compiler
