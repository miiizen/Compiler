#pragma once
#ifndef COMPILER_CODEGEN_H
#define COMPILER_CODEGEN_H

#include "AST.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include <map>

namespace Compiler{
    using std::unique_ptr;
    using std::shared_ptr;
    using namespace llvm;

    class Codegen : public Visitor {
        // Owns lots of core LLVM data. Needs to be passed into APIs
        LLVMContext context;
        // Helper object to generate IR instructions.  This will make my life 1000x easier
        IRBuilder<> builder;
        // Contains functions + global variables.  Can be seen as the top level structure
        unique_ptr<Module> module;
        // keeps track of values in the current scope. A symbol table
        std::map<std::string, Value*> namedValues;
    public:
        // Initialize builder because it's weird?
        Codegen()
            : builder(context) {}

        Value *logErrorV(const char *str);
        void visit(BlockAST* node) override;
        void visit(NumberAST* node) override;
        void visit(NameAST* node) override;
        void visit(ArrayAST* node) override;
        void visit(AssignmentAST* node) override;
        void visit(FuncCallAST* node) override;
        void visit(BinaryOpAST* node) override;
        void visit(UnaryOpAST* node) override;
        void visit(TernaryOpAST* node) override;
        void visit(IfAST* node) override;
        void visit(ForAST* node) override;
        void visit(FuncDefAST* node) override;
    };
}  // namespace Compiler

#endif //COMPILER_CODEGEN_H
