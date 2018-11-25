#pragma once
#ifndef __AST_H
#define __AST_H

#include <iostream>
#include <string>
#include <vector>
#include "token.h"

namespace Compiler {

	// Data types available in the language
	/*enum DataType {
		NUMERIC,
		STRING,
		ARRAY
	};*/

	enum class ASTType {
		BLOCK,
		NUMBER,
		NAME,
		ARRAY,
		ASSIGNMENT,
		FUNCCALL,
		BINARYOP,
		UNARYOP,
		TERNARYOP,
		IF,
		FOR
	};

	// Base AST node class
	class AST
	{
	public:
		virtual ~AST() = default;
		virtual const ASTType getType() = 0;
		// Hook into visitor class
		virtual void accept(Visitor &v) = 0;
	};

	// Represents a block with an arbitrary number of children
	class BlockAST : public AST {
		ASTType type = ASTType::BLOCK;
		std::vector<std::shared_ptr<AST>> children;
	public:
		BlockAST(std::vector<std::shared_ptr<AST>> children)
			: children(std::move(children)) {}
		virtual const ASTType getType() { return type; };
		std::vector<std::shared_ptr<AST>> getChildren() { return children; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<BlockAST>(this)); };
	};

	// Represents numeric literals.  Everything is a double at the moment??????
	class NumberAST : public AST {
		ASTType type = ASTType::NUMBER;
		double val;
	public:
		NumberAST(double val) : val(val) {}
		virtual const ASTType getType() { return type; };
		const double getVal() { return val; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<NumberAST>(this)); };

	};

	// Represents a variable
	class NameAST : public AST {
		ASTType type = ASTType::NAME;
		std::string name;
	public:
		NameAST(const std::string &name) : name(name) {}
		virtual const ASTType getType() { return type; };
		const std::string getName() { return name; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<NameAST>(this)); };
	};

	// Compound type
	class ArrayAST : public AST {
		ASTType type = ASTType::ARRAY;
		std::unique_ptr<AST> name;
		//DataType type;
	public:
		ArrayAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> vals)
			: name(std::move(name)), values(std::move(vals)) {}
		virtual const ASTType getType() { return type; };
		std::vector<std::shared_ptr<AST>> values;

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<ArrayAST>(this)); };
	};

	// Represents an assignment expression
	class AssignmentAST : public AST {
		ASTType type = ASTType::ASSIGNMENT;
		std::unique_ptr<AST> name, rhs;
	public:
		AssignmentAST(std::unique_ptr<AST> name, std::unique_ptr<AST> rhs)
			: name(std::move(name)), rhs(std::move(rhs)) {}
		virtual const ASTType getType() { return type; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<AssignmentAST>(this)); };
	};

	// Represents a function call
	class FuncCallAST : public AST {
		ASTType type = ASTType::FUNCCALL;
		std::unique_ptr<AST> name;
		std::vector<std::shared_ptr<AST>> args;
	public:
		FuncCallAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> args)
			: name(std::move(name)), args(std::move(args)) {}
		virtual const ASTType getType() { return type; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<FuncCallAST>(this)); };
	};

	// Represents binary operators.  Can have 2 children
	class BinaryOpAST : public AST {
		ASTType type = ASTType::BINARYOP;
		TokenType op;
		std::unique_ptr<AST> lhs, rhs;

	public:
		BinaryOpAST(TokenType op, std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs)
			: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
		virtual const ASTType getType() { return type; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<BinaryOpAST>(this)); };
	};

	// Represents a unary operator
	class UnaryOpAST : public AST {
		ASTType type = ASTType::UNARYOP;
		TokenType op;
		std::unique_ptr<AST> operand;

	public:
		UnaryOpAST(TokenType op, std::unique_ptr<AST> operand)
			: op(op), operand(std::move(operand)) {}
		virtual const ASTType getType() { return type; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<UnaryOpAST>(this)); };
	};

	// Represents a ternary operator
	class TernaryOpAST : public AST {
		ASTType type = ASTType::TERNARYOP;
		std::unique_ptr<AST> condition, thenArm, elseArm;
	public:
		TernaryOpAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenArm, std::unique_ptr<AST> elseArm)
			: condition(std::move(condition)), thenArm(std::move(thenArm)), 
			  elseArm(std::move(elseArm)) {}
		virtual const ASTType getType() { return type; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<TernaryOpAST>(this)); };
	};

	// If/else statement
	class IfAST : public AST {
		ASTType type = ASTType::IF;
		std::unique_ptr<AST> condition, thenBlock, elseBlock;
	public:
		IfAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenBlock, std::unique_ptr<AST> elseBlock)
			: condition(std::move(condition)), thenBlock(std::move(thenBlock)), 
			  elseBlock(std::move(elseBlock)) {}
		virtual const ASTType getType() { return type; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<IfAST>(this)); };
	};

	class ForAST : public AST {
		ASTType type = ASTType::FOR;
		// Should this be an AST class?
		std::string varName;
		std::unique_ptr<AST> start, end, step, body;

	public:
		ForAST(const std::string &varName, std::unique_ptr<AST> start,
			std::unique_ptr<AST> end,
			std::unique_ptr<AST> step,
			std::unique_ptr<AST> body)
			: varName(varName), start(std::move(start)), end(std::move(end)),
			step(std::move(step)), body(std::move(body)) {}
		virtual const ASTType getType() { return type; };

		// Visitor hook
		virtual void accept(Visitor &v) { v.visit(unique_ptr<ForAST>(this)); };
	};
}

#endif