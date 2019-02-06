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
        // Recursively generate code for children
        // TODO(JAMES) Return values from multiple statements? make whole block and return.  next few 
        for (const auto &child : node->getChildren()) {
            child->accept(this);
        }
    }

    void Codegen::visit(NumberAST *node)
    {
        // Somehow get this to the calling function?
        // Need to store in state as we cannot return from these functions.
        Value *val = ConstantFP::get(context, APFloat(node->getVal()));
        retVal = val;
    }

    void Codegen::visit(NameAST *node)
    {
        // Look variable up
        Value *val = namedValues[node->toString()];
        if (!val)
            logErrorV("Unknown variable name");
        retVal = val;

    }

    void Codegen::visit(ArrayAST *node)
    {

    }

    void Codegen::visit(AssignmentAST *node)
    {

    }

    void Codegen::visit(FuncCallAST *node)
    {
        // Get name of function with weird workaround
        node->getName()->accept(&nameGetter);
        std::string name = nameGetter.getLastName();

        // Look up name in global module table
        Function *calleeFunc = module->getFunction(name);
        // No function found
        if (!calleeFunc)
            logErrorV("Reference to unknown function");

        // Check number of args passed
        std::vector<shared_ptr<AST>> args = node->getArgs();
        if (calleeFunc->arg_size() != args.size())
            logErrorV("Incorrect number of arguments passed");//TODO(James) more descriptive error message

        // Recursively generate code for each argument
        std::vector<Value *> argValues;
        for(unsigned i = 0, e = args.size(); i != e; ++i) {
            args[i]->accept(this);
            argValues.push_back(retVal);
        }

        Value *val = builder.CreateCall(calleeFunc, argValues, "calltmp");
        retVal = val;

    }

    void Codegen::visit(BinaryOpAST *node)
    {
        // Get lhs and rhs somehow?!?!?!
        node->getLhs()->accept(this);
        Value *lhs = retVal;
        node->getRhs()->accept(this);
        Value *rhs = retVal;

        if (!lhs || !rhs)
            logErrorV("Missing an operand for binary operator");

        Value *val;

        switch (node->getOp()) {
            case PLUS:
                val = builder.CreateFAdd(lhs, rhs, "addtmp");
                retVal = val;
                break;
            case MINUS:
                val = builder.CreateFSub(lhs, rhs, "subtmp");
                retVal = val;
                break;
            case STAR:
                val = builder.CreateFMul(lhs, rhs, "multmp");
                retVal = val;
                break;
            case SLASH:
                val = builder.CreateFDiv(lhs, rhs, "divtmp");
                retVal = val;
                break;
            case LESS:
                // Returns 1 bit int
                lhs = builder.CreateFCmpULT(lhs, rhs, "cmptmp");
                // Convert integer into unsigned float
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));
                retVal = val;
                break;

                //TODO(James) implement all binary operators
            default:
                logErrorV("Invalid binary operator");
                retVal = nullptr;
                break;
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
        // Get if condition
        node->getCond()->accept(this);
        Value *conditionVal = retVal;

        if (!conditionVal){
            logErrorV("No condition");
            retVal = nullptr;
            return;
        }

        // convert condition to bool.  NE 0.0
        conditionVal = builder.CreateFCmpONE(conditionVal, ConstantFP::get(context, APFloat(0.0)), "ifcond");

        Function *parentFunc = builder.GetInsertBlock()->getParent();

        // Create blocks for if and else
        // insert then at end of function
        BasicBlock *thenBlock = BasicBlock::Create(context, "then", parentFunc);
        BasicBlock *elseBlock = BasicBlock::Create(context, "else");
        BasicBlock *mergeBlock = BasicBlock::Create(context, "ifcont");
        // emit conditional branch to choose between them
        builder.CreateCondBr(conditionVal, thenBlock, elseBlock);

        // Emit then value
        builder.SetInsertPoint(thenBlock);

        node->getThen()->accept(this);
        Value *thenVal = retVal;
        if (!thenVal) {
            logErrorV("No then value");
            retVal = nullptr;
            return;
        }
        // In LLVM IR all basic blocks must be terminated with a branch/return.  All control flow must be explicit
        builder.CreateBr(mergeBlock);
        // Set up Phi block TODO(JAMES) UNDERSTAND THIS!
        thenBlock = builder.GetInsertBlock();


        // Emit else block
        // TODO(James) optional else??
        auto elseTree = node->getElse();
        Value *elseVal;
        parentFunc->getBasicBlockList().push_back(elseBlock);
        builder.SetInsertPoint(elseBlock);
        if (elseTree) {
            elseTree->accept(this);
            elseVal = retVal;
            if (!elseVal) {
                logErrorV("No then value");
                retVal = nullptr;
                return;
            }

        }
        builder.CreateBr(mergeBlock);
        // codegen of else can change current block update for phi TODO(JAMES) UNDERSTAND
        elseBlock = builder.GetInsertBlock();

        // Emit merge code
        parentFunc->getBasicBlockList().push_back(mergeBlock);
        builder.SetInsertPoint(mergeBlock);
        PHINode *phi = builder.CreatePHI(Type::getDoubleTy(context), 2, "iftmp");

        phi->addIncoming(thenVal, thenBlock);
        if (elseTree)
            phi->addIncoming(elseVal, elseBlock);
        //return phi as value computed by expression
        retVal = phi;

    }

    void Codegen::visit(ForAST *node)
    {
        // Get start value
        node->getStart()->accept(this);
        Value *startVal = retVal;
        if (!startVal) {
            logErrorV("No start value found");
            retVal = nullptr;
            return;
        }

        // Set up basic block for loop body
        Function *parentFunc = builder.GetInsertBlock()->getParent();
        BasicBlock *preheaderBlock = builder.GetInsertBlock();
        BasicBlock *loopBlock = BasicBlock::Create(context, "loop", parentFunc);

        // finish with explicit fall through to loop block
        builder.CreateBr(loopBlock);

        // Begin insertion into loop block
        builder.SetInsertPoint(loopBlock);

        // Start PHI node with entry for start value
        PHINode *var = builder.CreatePHI(Type::getDoubleTy(context), 2, node->getVarName());
        var->addIncoming(startVal, preheaderBlock);

        // Emit code for loop body
        // Use phi node for the loop variable.  save it so it can be restored
        Value *oldLoopVarVal = namedValues[node->getVarName()];
        namedValues[node->getVarName()] = var;

        // Emit code for loop body
        node->getBody()->accept(this);
        if (!retVal) {
            logErrorV("No loop body generated");
            retVal = nullptr;
            return;
        }

        // Emit step value
        Value *stepVal = nullptr;
        // getStep() removes from AST.  Is this what I wanted to do..?
        auto step = node->getStep();
        if (step) {
            step->accept(this);
            stepVal = retVal;
            if (!stepVal) {
                logErrorV("Expected a step value");
                retVal = nullptr;
                return;
            }
        } else {
            // If not specified, use 1.0
            stepVal = ConstantFP::get(context, APFloat(1.0));
        }
        // Value of loop variable on next iteration
        Value *nextVar = builder.CreateFAdd(var, stepVal, "nextvar");

        // End condition
        node->getEnd()->accept(this);
        Value *endCondition = retVal;
        if (!endCondition) {
            logErrorV("Expected an end condition");
            retVal = nullptr;
            return;
        }

        // Convert condition to bool
        endCondition = builder.CreateFCmpONE(endCondition, ConstantFP::get(context, APFloat(0.0)), "loopcond");

        // Create post loop block and insert
        BasicBlock *loopEndBlock = builder.GetInsertBlock();
        BasicBlock *afterBlock = BasicBlock::Create(context, "afterloop", parentFunc);

        // Insert conditional into end of block
        builder.CreateCondBr(endCondition, loopBlock, afterBlock);

        // Insert any new code in the post loop block
        builder.SetInsertPoint(afterBlock);

        // Create exit block - decide to loop again or continue based on PHI node
        // update PHI
        var->addIncoming(nextVar, loopEndBlock);

        // Restore unshadowed variable
        if (oldLoopVarVal)
            namedValues[node->getVarName()] = oldLoopVarVal;
        else
            namedValues.erase(node->getVarName());

        // For should always return 0.0
        retVal = Constant::getNullValue(Type::getDoubleTy(context));
    }

    void Codegen::visit(FuncDefAST *node)
    {
        // ---- PROTOTYPE ----
        // Extract bits from node
        std::vector<shared_ptr<AST>> args = node->getArgs();
        // Get name of function with weird workaround
        node->getName()->accept(&nameGetter);
        std::string name = nameGetter.getLastName();

        // NO PROTOTYPES!
        // All types are doubles for now
        std::vector<Type*> doubles(args.size(), Type::getDoubleTy(context));

        FunctionType *ft = FunctionType::get(Type::getDoubleTy(context), doubles, false);

        // not sure about external linkage - however does mean it is callable outside of current module
        Function *func = Function::Create(ft, Function::ExternalLinkage, name, module.get());

        // Set names of args to those in code - not necessary and doesn't work!
        unsigned i = 0;
        for (auto &arg : func->args()) {
            args[i++]->accept(&nameGetter);
            std::string argName = nameGetter.getLastName();
            arg.setName(argName);
        }
        // return IR
        //retFunc = func;

        // ---- FUNCTION ----
        Function *thisFunc = module->getFunction(name);
        if (!thisFunc)
            logErrorV("Function genration failed.");
        //TODO(exit???)

        if(!thisFunc->empty())
            logErrorV("Function definition already found");

        // Create new basic block
        BasicBlock *base = BasicBlock::Create(context, "entry", thisFunc);
        builder.SetInsertPoint(base);

        // Record function args in the named values (new scope)
        namedValues.clear();
        for (auto &arg : thisFunc->args())
            namedValues[arg.getName()] = &arg;

        // Finish function
        node->getBod()->accept(this);
        Value* returnVal = retVal;
        // No body
        if(!returnVal) {
            thisFunc->eraseFromParent();
            retFunc = nullptr;
        }

        builder.CreateRet(returnVal);

        // Validate code
        verifyFunction(*thisFunc);

        // Optimise function
        //fpm->run(*thisFunc);
        thisFunc->viewCFG();

        retFunc = thisFunc;
    }

} // namespace Compiler