#include "codegen.h"
#include "AST.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
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
        // TODO(JAMES) Return values from multiple statements? make whole block and return.
        // Append BasicBlocks to function blocklist.  The blocks MUST end with a branch to the next block
        // REMEMBER: ALL BASIC BLOCKS MUST BE TERMINATED WITH A RETURN OR BRANCH! The LLVM verifier will fail otherwise
        // Also, create the blocks you think you'll need early on!

        // Get the function we are inserting into
        BasicBlock *currentBlock = builder.GetInsertBlock();
        // If null, we are at the top level of the program.
        // We want to generate function definitions in this case
        // The structure of the program (just function definitions at the top level) should be verified at some point,
        // //probably earlier on.
        if (!currentBlock) {
            for (const auto &child : node->getChildren()) {
                if(child->getType() != ASTType::FUNCDEF) {
                    logErrorV("Expected function definitions at the top level");
                }
                child->accept(this);
            }
            return;
        }

        // If there is a parent, we gt to this point and assume we are in a function
        Function *parentFunc = currentBlock->getParent();

        // Generate code recursively
        for (const auto &child : node->getChildren()) {
            BasicBlock *lastBlock = &parentFunc->back();
            // Create block at the end of the parent
            BasicBlock *nextBlock = BasicBlock::Create(context, "block", parentFunc);
            // Create branch to nextBlock from lastBlock
            builder.SetInsertPoint(lastBlock);
            builder.CreateBr(nextBlock);
            // Move builder to nextBlock - GetInsertBlock for recursive generation of blocks that might change their
            // parents?
            builder.SetInsertPoint(nextBlock);

            // emit code for child
            child->accept(this);
            Value * line = retVal;
            if (!line) {
                logErrorV("No value returned from child in block!");
                retVal = nullptr;
                return;
            }
        }
        // Create some sort of exit branch?
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
        // Load the value from memory
        retVal = builder.CreateLoad(val, node->toString());
    }

    void Codegen::visit(ArrayAST *node)
    {

    }

    void Codegen::visit(AssignmentAST *node)
    {
        // Generate Value* for RHS
        node->getRhs()->accept(this);
        Value *val = retVal;
        if (!val) {
            logErrorV("There must be an expression on RHS.");
            retVal = nullptr;
            return;
        }

        // Look up name
        node->getName()->accept(&nameGetter);
        std::string name = nameGetter.getLastName();
        Value *var = namedValues[name];
        // If var cannot be found, define.  If it can, redefine.
        if (!var) {
            // Add new variables to values table
            // Get parent function/scope
            Function *parentFunc = builder.GetInsertBlock()->getParent();
            // Create alloca for variable
            AllocaInst *alloca = CreateEntryBlockAlloca(parentFunc, name);
            // Store and place in name table
            builder.CreateStore(val, alloca);
            namedValues[name] = alloca;
        } else {
            // store in memory
            builder.CreateStore(val, var);
        }
        // allocation expression evaluates to the RHS value
        retVal = val;
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
        // Get lhs and rhs
        node->getLhs()->accept(this);
        Value *lhs = retVal;
        node->getRhs()->accept(this);
        Value *rhs = retVal;

        if (!lhs || !rhs)
            logErrorV("Missing an operand for binary operator");

        Value *val;

        switch (node->getOp()) {
            /* ---- Arithmetic ---- */
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
            //TODO(James) powers?
            case MOD:
                // Signed??
                val = builder.CreateSRem(lhs, rhs, "remtmp");
                retVal = val;
                break;
            /* ---- Comparison ---- */
            case LESS:
                // Returns 1 bit int
                lhs = builder.CreateFCmpULT(lhs, rhs, "cmptmp");
                // Convert integer into unsigned float
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));
                retVal = val;
                break;
            case GREATER:
                lhs = builder.CreateFCmpUGT(lhs, rhs, "cmptmp");
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));
                retVal = val;
                break;
            case EQ:
                lhs = builder.CreateFCmpUEQ(lhs, rhs, "cmptmp");
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));
                retVal = val;
                break;
            case NEQ:
                lhs = builder.CreateFCmpUNE(lhs, rhs, "cmptmp");
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));
                retVal = val;
                break;
            case GREQ:
                lhs = builder.CreateFCmpUGE(lhs, rhs, "cmptmp");
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));
                retVal = val;
                break;
            case LEQ:
                lhs = builder.CreateFCmpULE(lhs, rhs, "cmptmp");
                val = builder.CreateUIToFP(lhs, Type::getDoubleTy(context));
                retVal = val;
                break;
            /* ---- Logical ---- */
            //TODO(James) implement all binary operators
            default:
                logErrorV("Invalid binary operator");
                retVal = nullptr;
                break;
        }

    }

    void Codegen::visit(UnaryOpAST *node)
    {
        // Get operand
        node->getOperand()->accept(this);
        Value *operand = retVal;

        if (!operand)
            logErrorV("Missing the operand for the unary operator");

        Value *val;
        // One for incrementing and decrementing
        Value *one = ConstantFP::get(context, APFloat(1.0));


        switch (node->getOp()) {
            /* ---- Arithmetic ---- */
            case INC:
                val = builder.CreateFAdd(operand, one);
                retVal = val;
                break;
            case DEC:
                val = builder.CreateFSub(operand, one);
                retVal = val;
                break;

            default:
                logErrorV("Invalid unary operator");
                retVal = nullptr;
                break;
        }

    }

    void Codegen::visit(TernaryOpAST *node)
    {
        // TODO(James) Same as If/else but with single expressions?
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

        thenBlock = builder.GetInsertBlock();

        // Emit else block
        // If there is no else block, do not add anything to PHI node
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

        // Set up basic blocks for loop
        Function *parentFunc = builder.GetInsertBlock()->getParent();

        // Create alloca for variable in entry block
        AllocaInst *alloca = CreateEntryBlockAlloca(parentFunc, node->getVarName());

        // Store start value in alloca
        builder.CreateStore(startVal, alloca);

        //BasicBlock *preheaderBlock = builder.GetInsertBlock();
        BasicBlock *loopBlock = BasicBlock::Create(context, "loop", parentFunc);

        // finish with explicit fall through to loop block
        builder.CreateBr(loopBlock);

        // Begin insertion into loop block
        builder.SetInsertPoint(loopBlock);

        // Emit code for loop body
        AllocaInst *oldLoopVarVal = namedValues[node->getVarName()];
        namedValues[node->getVarName()] = alloca;

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
        // Reload increment and restore alloca. handles case where loop body modifies the variable
        Value *curVar = builder.CreateLoad(alloca, node->getVarName());
        Value *nextVar = builder.CreateFAdd(curVar, stepVal, "nextvar");
        builder.CreateStore(nextVar, alloca);

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
        BasicBlock *afterBlock = BasicBlock::Create(context, "afterloop", parentFunc);

        // Insert conditional into end of block
        builder.CreateCondBr(endCondition, loopBlock, afterBlock);

        // Insert any new code in the post loop block
        builder.SetInsertPoint(afterBlock);

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
        for (auto &arg : thisFunc->args()){
            // Create alloca for variable
            AllocaInst *alloca = CreateEntryBlockAlloca(thisFunc, arg.getName());
            // Store initial value in alloca
            builder.CreateStore(&arg, alloca);
            // add arguments to symbol table
            namedValues[arg.getName()] = alloca;
        }

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
        fpm->run(*thisFunc);
        thisFunc->viewCFG();

        retFunc = thisFunc;
    }

    AllocaInst *Codegen::CreateEntryBlockAlloca(Function *func, const std::string &varName)
    {
        // Create temporary builder pointing to the entry of the function, then create an alloca with the correct name
        // and return
        IRBuilder<> tempBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());
        return tempBuilder.CreateAlloca(Type::getDoubleTy(context), 0, varName);
    }

    int Codegen::emitObjCode(std::string filename)
    {
        // Initialise all targets
        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        // Set target triple
        auto targetTriple = sys::getDefaultTargetTriple();
        module->setTargetTriple(targetTriple);

        std::string error;
        auto target = TargetRegistry::lookupTarget(targetTriple, error);

        // Print error and exit if requested target couldn't be found
        if (!target) {
            errs() << error;
            return 1;
        }
        // Basic generic CPU
        auto CPU = "generic";
        auto Features = "";

        TargetOptions opt;
        auto RM = Optional<Reloc::Model>();
        auto targetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);

        // Configure the module for optimization
        module->setDataLayout(targetMachine->createDataLayout());

        // Emit object code
        std::error_code ec;
        raw_fd_ostream dest(filename, ec, sys::fs::F_None);

        if (ec) {
            errs() << "Could not open file: " << ec.message();
            return 1;
        }
        // Pass emits object code
        legacy::PassManager pass;
        auto fileType = TargetMachine::CGFT_ObjectFile;

        if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
            errs() << "TargetMachine can't emit a file of this type";
            return 1;
        }

        pass.run(*module);
        dest.flush();

        outs() << "Wrote " << filename << "\n";

        return 0;

    }

} // namespace Compiler