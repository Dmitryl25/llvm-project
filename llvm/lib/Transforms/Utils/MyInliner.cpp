#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/MyInliner.h"

using namespace llvm;


PreservedAnalyses MyInlinePass::run(Function& F, FunctionAnalysisManager& FM) {
    bool IsChanged = false;
    std::vector<CallInst*> Calls;
    // Цикл проходит по всем инструкциям и ищет подходящие call
    for (BasicBlock& BB : F) {

        // Обход инструкций внутри блока
        for (Instruction& I : BB) {
            // Проверка, что инструкция - это вызов функции, причем пропускаем рекурсию, не void и ф-ции с параметрами
            
            if (auto* Call = dyn_cast<CallInst>(&I)) {
                Function* CalledFunc = Call->getCalledFunction();

                if (CalledFunc == &F) continue;

                if (!CalledFunc || CalledFunc->isDeclaration()) {
                    continue;
                }

                if (!CalledFunc->getReturnType()->isVoidTy() || CalledFunc->arg_size() != 0) {
                    continue;
                }
                // Добавляем подходящую функцию для последующего инлайнинга
                Calls.push_back(Call);
            }
        }
    }

    for (CallInst* Call : Calls) {
        Function* CalledFunc = Call->getCalledFunction();
        BasicBlock* CallBB = Call->getParent();

        // Разделяем текущий блок на две части (до вызова функций включительно и после)
        Instruction* Next = Call->getNextNode();
        BasicBlock* AfterBB = CallBB->splitBasicBlock(Next, "post.call");

        // Убираем автоматически созданный после разделения переход br
        CallBB->getTerminator()->eraseFromParent();


        ValueToValueMapTy VMap;
        SmallVector<BasicBlock*, 8> ClonedBlocks;
        // Перебираем все базовые блоки вызываемой функций с целью их клонирования
        for (BasicBlock& BB : *CalledFunc) {
            BasicBlock* ClonedBB = CloneBasicBlock(&BB, VMap, "cloned", &F);
            VMap[&BB] = ClonedBB;
            ClonedBlocks.push_back(ClonedBB);
        }

        // Ремапим инструкции (обновляем их связи, используя VMap) и заменяем return на переход
        for (BasicBlock* BB : ClonedBlocks) {
            for (Instruction& I : *BB) {
                RemapInstruction(&I, VMap,
                    RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
            }
            // добавляем переход на AfterBB из инлайновского кода и удаляем ret из него же
            if (auto* RI = dyn_cast<ReturnInst>(BB->getTerminator())) {
                IRBuilder<>(BB).CreateBr(AfterBB);
                RI->eraseFromParent();
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