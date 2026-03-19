#ifndef LLVM_TRANSFORMS_UTILS_MYINLINER_H
#define LLVM_TRANSFORMS_UTILS_MYINLINER_H

#include "llvm/IR/PassManager.h"

namespace llvm {

    class MyInlinePass : public PassInfoMixin<MyInlinePass> {
    public:
        PreservedAnalyses run(Function& F, FunctionAnalysisManager& FM);
    };

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_MYINLINER_H
