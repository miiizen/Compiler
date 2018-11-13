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
		UNARY,
		TERNARYOP,
		IF,
		FOR
	};

	// Base AST node class
	class AST
	{
	public:
		virtual ~AST() = default;
		virtual ASTType getType() = 0;
	};

	// Represents a block with an arbitrary number of children
	class BlockAST : public AST {
		std::vector<std::shared_ptr<AST>> children;
		ASTType type = ASTType::BLOCK;
	public:
		BlockAST(std::vector<std::shared_ptr<AST>> children)
			: children(std::move(children)) {}
		virtual ASTType getType() { return type; };
	};

	// Represents numeric literals.  Everything is a double at the moment??????
	class NumberAST : public AST {
		double val;
		ASTType type = ASTType::NUMBER;
	public:
		NumberAST(double val) : val(val) {}
		virtual ASTType getType() { return type; };
	};

	// Represents a variable
	class NameAST : public AST {
		ASTType type = ASTType::NAME;
	public:
		std::string name;
		NameAST(const std::string &name) : name(name) {}
		virtual ASTType getType() { return type; };
	};

	// Compound type
	class ArrayAST : public AST {
		ASTType type = ASTType::ARRAY;
		std::unique_ptr<AST> name;
		//DataType type;
		std::vector<std::shared_ptr<AST>> values;
	public:
		ArrayAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> vals)
			: name(std::move(name)), values(std::move(vals)) {}
		virtual ASTType getType() { return type; };
	};

	// Represents an assignment expression
	class AssignmentAST : public AST {
		ASTType type = ASTType::ASSIGNMENT;
		std::unique_ptr<AST> name, rhs;
	public:
		AssignmentAST(std::unique_ptr<AST> name, std::unique_ptr<AST> rhs)
			: name(std::move(name)), rhs(std::move(rhs)) {}
		virtual ASTType getType() { return type; };
	};

	// Represents a function call
	class FuncCallAST : public AST {
		ASTType type = ASTType::FUNCCALL;
		std::unique_ptr<AST> name;
		std::vector<std::shared_ptr<AST>> args;
	public:
		FuncCallAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> args)
			: name(std::move(name)), args(std::move(args)) {}
		virtual ASTType getType() { return type; };
	};

	// Represents binary operators.  Can have 2 children
	class BinaryOpAST : public AST {
		ASTType type = ASTType::BINARYOP;
		TokenType op;
		std::unique_ptr<AST> lhs, rhs;

	public:
		BinaryOpAST(TokenType op, std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs)
			: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
		virtual ASTType getType() { return type; };
	};

	// Represents a unary operator
	class UnaryOpAST : public AST {
		ASTType type = ASTType::PREFIXOP;
		TokenType op;
		std::unique_ptr<AST> operand;

	public:
		UnaryOpAST(TokenType op, std::unique_ptr<AST> operand)
			: op(op), operand(std::move(operand)) {}
		virtual ASTType getType() { return type; };
	};

	// Represents a ternary operator
	class TernaryOpAST : public AST {
		ASTType type = ASTType::TERNARYOP;
		std::unique_ptr<AST> condition, thenArm, elseArm;
	public:
		TernaryOpAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenArm, std::unique_ptr<AST> elseArm)
			: condition(std::move(condition)), thenArm(std::move(thenArm)), 
			  elseArm(std::move(elseArm)) {}
		virtual ASTType getType() { return type; };
	};

	// If/else statement
	class IfAST : public AST {
		ASTType type = ASTType::IF;
		std::unique_ptr<AST> condition, thenBlock, elseBlock;
	public:
		IfAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenBlock, std::unique_ptr<AST> elseBlock)
			: condition(std::move(condition)), thenBlock(std::move(thenBlock)), 
			  elseBlock(std::move(elseBlock)) {}
		virtual ASTType getType() { return type; };
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
		virtual ASTType getType() { return type; };
	};
}

#endif