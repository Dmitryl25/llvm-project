#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/MyInliner.h"

using namespace llvm;

// Функция для проверки, что инструкция - это вызов функции, причем пропускаем рекурсию, не void и ф-ции с параметрами
static bool isInlineCandidate(Function& F, CallInst* Call) {
    
    Function* CalledFunc = Call->getCalledFunction();
    if (!CalledFunc || CalledFunc->isDeclaration()) {
        return false;
    }

    if (CalledFunc == &F) {
        return false;
    }

    if (!CalledFunc->getReturnType()->isVoidTy() || CalledFunc->arg_size() != 0) {
        return false;
    }

    return true;
}


PreservedAnalyses MyInlinePass::run(Function& F, FunctionAnalysisManager& FM) {
    bool IsChanged = false;
    SmallVector<CallInst*, 16> Calls;
    // Цикл проходит по всем блокам и ищет подходящие call
    for (BasicBlock& BB : F) {

        // Обход инструкций внутри блока
        for (Instruction& I : BB) {
            
            if (auto* Call = dyn_cast<CallInst>(&I)) {
                if (isInlineCandidate(F, Call)) {
                    Calls.push_back(Call);
                }
            }
        }
    }

    for (CallInst* Call : Calls) {
        BasicBlock* CallBB = Call->getParent();

        // Разделяем текущий блок на две части (до вызова функций включительно и после)
        Instruction* Next = Call->getNextNode();
        BasicBlock* AfterBB = CallBB->splitBasicBlock(Next, "post.call");

        // Убираем автоматически созданный после разделения переход br
        CallBB->getTerminator()->eraseFromParent();


        ValueToValueMapTy VMap;
        SmallVector<BasicBlock*, 8> ClonedBlocks;
        // Перебираем все базовые блоки вызываемой функций с целью их клонирования
        Function* CalledFunc = Call->getCalledFunction();

        BasicBlock* LastInsertedBB = CallBB;

        for (BasicBlock& BB : *CalledFunc) {
            BasicBlock* ClonedBlock = CloneBasicBlock(&BB, VMap, "cloned", &F);
            // Каждый новый клон двигаем сразу после предыдущего блока, чтобы всё шло в том же порядке 
            ClonedBlock->moveAfter(LastInsertedBB);
            LastInsertedBB = ClonedBlock;
            ClonedBlocks.push_back(ClonedBlock);
            VMap[&BB] = ClonedBlocks.back();
        }

        AfterBB->moveAfter(LastInsertedBB);

        // Ремапим инструкции (обновляем их связи, используя VMap)
        for (BasicBlock* BB : ClonedBlocks) {
            for (Instruction& I : *BB) {
                RemapInstruction(&I, VMap,RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
            }
            // добавляем переход на AfterBB из инлайновского кода и удаляем ret из него же
            if (auto* RetInst = dyn_cast<ReturnInst>(BB->getTerminator())) {
                IRBuilder<> Builder(RetInst);
                Builder.CreateBr(AfterBB);
                RetInst->eraseFromParent();
            }
        }

        // Берем начало заинлайненого блока
        BasicBlock* EntryBB = cast<BasicBlock>(VMap[&CalledFunc->getEntryBlock()]);
        // Добавляем переход в это начало блока
        BranchInst::Create(EntryBB, CallBB);

        // Удаляем сам вызов
        Call->eraseFromParent();

        IsChanged = true;
    }

    return IsChanged ? PreservedAnalyses::none() : PreservedAnalyses::all();

}