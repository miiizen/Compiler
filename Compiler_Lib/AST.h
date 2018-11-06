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

	// Base AST node class
	class AST
	{
	public:
		virtual ~AST() = default;
	};

	// Represents a block with an arbitrary number of children
	class BlockAST : public AST {
		std::vector<std::shared_ptr<AST>> children;
	public:
		BlockAST(std::vector<std::shared_ptr<AST>> children)
			: children(std::move(children)) {}
	};

	// Represents numeric literals.  Everything is a double at the moment??????
	class NumberAST : public AST {
		double val;
	public:
		NumberAST(double val) : val(val) {}
	};

	// Represents a variable
	class NameAST : public AST {

	public:
		std::string name;
		NameAST(const std::string &name) : name(name) {}
	};

	// Compound type
	class ArrayAST : public AST {
		std::unique_ptr<AST> name;
		//DataType type;
		std::vector<std::shared_ptr<AST>> values;
	public:

		ArrayAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> vals)
			: name(std::move(name)), values(std::move(vals)) {}
	};

	// Represents an assignment expression
	class AssignmentAST : public AST {
		std::unique_ptr<AST> name, rhs;

	public:
		AssignmentAST(std::unique_ptr<AST> name, std::unique_ptr<AST> rhs)
			: name(std::move(name)), rhs(std::move(rhs)) {}
	};

	// Represents a function call
	class FuncCallAST : public AST {
		std::unique_ptr<AST> name;
		std::vector<std::shared_ptr<AST>> args;

	public:
		FuncCallAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> args)
			: name(std::move(name)), args(std::move(args)) {}
	};

	// Represents binary operators.  Can have 2 children
	class BinaryOpAST : public AST {
		TokenType op;
		std::unique_ptr<AST> lhs, rhs;

	public:
		BinaryOpAST(TokenType op, std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs)
			: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	};

	// Represents a unary prefix operator
	class PrefixOpAST : public AST {
		TokenType op;
		std::unique_ptr<AST> operand;

	public:
		PrefixOpAST(TokenType op, std::unique_ptr<AST> operand)
			: op(op), operand(std::move(operand)) {}
	};

	// Represents a unary postfix operator
	// TODO Can we merge both unary operators? they are basically the same ast wise
	class PostfixOpAST : public AST {
		TokenType op;
		std::unique_ptr<AST> operand;

	public:
		PostfixOpAST(TokenType op, std::unique_ptr<AST> operand)
			: op(op), operand(std::move(operand)) {}
	};

	// Represents a ternary operator
	class TernaryOpAST : public AST {
		std::unique_ptr<AST> condition, thenArm, elseArm;
	public:
		TernaryOpAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenArm, std::unique_ptr<AST> elseArm)
			: condition(std::move(condition)), thenArm(std::move(thenArm)), elseArm(std::move(elseArm)) {}
	};

	// If/else statement
	class IfAST : public AST {
		std::unique_ptr<AST> condition, thenBlock, elseBlock;
	public:
		IfAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenBlock, std::unique_ptr<AST> elseBlock)
			: condition(std::move(condition)), thenBlock(std::move(thenBlock)), elseBlock(std::move(elseBlock)) {}
	};
}

#endif