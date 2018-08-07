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
	class VariableAST : public AST {
		std::string name;

	public:
		VariableAST(const std::string &name) : name(name) {}
	};

	// Represents binary operators.  Can have 2 children
	class BinaryOpAST : public AST {
		std::string op;
		std::unique_ptr<AST> lhs, rhs;

	public:
		BinaryOpAST(std::string op, std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs)
			: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	};
}

#endif