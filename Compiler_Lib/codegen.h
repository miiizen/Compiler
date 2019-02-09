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
#include "llvm/IR/Instructions.h"
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
#include "llvm/Transforms/Utils.h"
#include <map>

namespace Compiler{
    using std::unique_ptr;
    using std::shared_ptr;
    using namespace llvm;

    // Class to get names since they are hidden in another AST class.
    // I have no idea whether this is the correct way of doing things, but I do like a visitor
    // TODO(James) ok all names should be strings, this class is ridiculous
    class NameGetter : public Visitor {
        // store the last name accessed
        std::string lastName;
    public:
        std::string getLastName() { return lastName; };
        void visit(BlockAST* node) override { /* No name */ };
        void visit(NumberAST* node) override { /* No name */ };
        void visit(NameAST* node) override { lastName = node->toString(); };
        void visit(ArrayAST* node) override { node->getName()->accept(this); };
        void visit(AssignmentAST* node) override { node->getName()->accept(this); };
        void visit(FuncCallAST* node) override { node->getName()->accept(this); };
        void visit(BinaryOpAST* node) override { /* No name */ };
        void visit(UnaryOpAST* node) override { /* No name */ };
        void visit(TernaryOpAST* node) override { /* No name */ };
        void visit(IfAST* node) override { /* No name */ };
        void visit(ForAST* node) override { /* No name */ };
        void visit(FuncDefAST* node) override { node->getName()->accept(this); };
    };

    class Codegen : public Visitor {
        // Owns lots of core LLVM data. Needs to be passed into APIs
        LLVMContext context;
        // Helper object to generate IR instructions.  This will make my life 1000x easier
        IRBuilder<> builder;
        // Contains functions + global variables.  Can be seen as the top level structure
        unique_ptr<Module> module;
        // keeps track of values in the current scope. A symbol table
        std::map<std::string, AllocaInst*> namedValues;
        // Pass manager to optimize functions
        std::unique_ptr<legacy::FunctionPassManager> fpm;
        // Since we cannot return, store values and functions which the code generation functions should return in here
        Value * retVal;
        Function * retFunc;
        // Visitor to extract names
        NameGetter nameGetter;
        // Helper function to create an alloca instruction in the entry block of a function
        AllocaInst *CreateEntryBlockAlloca(Function *func, const std::string &varName);
    public:
        // Initialize builder, module with context.  also init pointers to nullptr
        Codegen() : builder(context), module(std::make_unique<Module>("JIT", context)), fpm(std::make_unique<legacy::FunctionPassManager>(module.get())), retVal(nullptr), retFunc(nullptr) {
            // Promote allocas to registers (speed)
            fpm->add(createPromoteMemoryToRegisterPass());
            // simple peephole optimizations
            fpm->add(createInstructionCombiningPass());
            // re-associate expressions
            fpm->add(createReassociatePass());
            // eliminate common sub-expressions
            fpm->add(createGVNPass());
            // simplify control flow graph
            fpm->add(createCFGSimplificationPass());
            // init pass manager
            fpm->doInitialization();
        }

        int emitObjCode(std::string filename);

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
