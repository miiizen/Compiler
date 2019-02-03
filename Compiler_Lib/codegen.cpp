#include "codegen.h"
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

namespace Compiler {


    Value *Codegen::logErrorV(const char *str)
    {
        fprintf(stderr, "Error: %s\n", str);
        return nullptr;
    }

    void Codegen::visit(BlockAST *node)
    {

    }

    void Codegen::visit(NumberAST *node)
    {
        double num = node->getVal();
        // Somehow get this to the calling function?
        // Need to store in state as we cannot return from these functions.
        Value *val = ConstantFP::get(context, APFloat(num));
    }

    void Codegen::visit(NameAST *node)
    {
        // Look variable up
        Value *val = namedValues[node->getName()];
        if (!val)
            logErrorV("Unknown variable name");
        // Return data somehow?!?!?

    }

    void Codegen::visit(ArrayAST *node)
    {

    }

    void Codegen::visit(AssignmentAST *node)
    {

    }

    void Codegen::visit(FuncCallAST *node)
    {

    }

    void Codegen::visit(BinaryOpAST *node)
    {
        // Get lhs and rhs somehow?!?!?!
        Value *lhs;
        Value *rhs;

        if (!lhs || !rhs)
            logErrorV("Missing an operand for binary operator");

        Value *val;

        switch (node->getOp()) {
            case PLUS:
                val = builder.CreateFAdd(lhs, rhs, "addtmp");
            case MINUS:
                val = builder.CreateFSub(lhs, rhs, "subtmp");
            case STAR:
                val = builder.CreateFMul(lhs, rhs, "multmp");
            case SLASH:
                val = builder.CreateFDiv(lhs, rhs, "divtmp");
            case LESS:
                // Returns 1 bit int
                lhs = builder.CreateFCmpULT(lhs, rhs, "cmptmp");
                // Convert integer into unsigned float
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));

                //TODO(James) implement all binary operators
            default:
                logErrorV("Invalid binary operator");
        }

    }

    void Codegen::visit(UnaryOpAST *node)
    {

    }

    void Codegen::visit(TernaryOpAST *node)
    {

    }

    void Codegen::visit(IfAST *node)
    {

    }

    void Codegen::visit(ForAST *node)
    {

    }

    void Codegen::visit(FuncDefAST *node)
    {

    }

} // namespace Compiler