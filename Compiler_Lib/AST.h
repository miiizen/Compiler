#pragma once
#ifndef __AST_H
#define __AST_H

#include <iostream>
#include <string>
#include "token.h"

namespace Compiler {
	// Base AST node class
	class AST
	{
	public:
		virtual ~AST() = default;
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

	// Represents an assignment expression
	class AssignmentAST : public AST {
		std::string name;
		std::unique_ptr<AST> rhs;

	public:
		AssignmentAST(std::string name, std::unique_ptr<AST> rhs)
			: name(name), rhs(std::move(rhs)) {}
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
		std::unique_ptr<AST> condition;
		std::unique_ptr<AST> thenArm;
		std::unique_ptr<AST> elseArm;
	public:
		TernaryOpAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenArm, std::unique_ptr<AST> elseArm)
			: condition(std::move(condition)), thenArm(std::move(thenArm)), elseArm(std::move(elseArm)) {}
	};
}

#endif