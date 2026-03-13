#include <iostream>
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

namespace {

class MaybeUnusedVisitor : public RecursiveASTVisitor<MaybeUnusedVisitor> {
public:
    explicit MaybeUnusedVisitor(ASTContext* ctx)
        : Context(ctx) {}

    bool VisitVarDecl(VarDecl* VD) {
        handleVarDecl(VD);
        return true;
    }

    bool VisitParmVarDecl(ParmVarDecl* PD) {
        handleVarDecl(PD);
        return true;
    }

private:
    ASTContext* Context;

    void handleVarDecl(VarDecl* VD) {
        if (!VD) return;

        if (!VD->isUsed() && !VD->hasAttr<UnusedAttr>()) {
            VD->addAttr(UnusedAttr::CreateImplicit(
                *Context,
                VD->getBeginLoc()
            ));
        }
    }
};

class MaybeUnusedConsumer : public ASTConsumer {
public:
    explicit MaybeUnusedConsumer(ASTContext* ctx)
        : Visitor(ctx) {
    }

    void HandleTranslationUnit(ASTContext& ctx) override {
        Visitor.TraverseDecl(ctx.getTranslationUnitDecl());
    }

private:
    MaybeUnusedVisitor Visitor;
};

class MaybeUnusedAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(
        CompilerInstance& CI,
        llvm::StringRef) override {
        return std::make_unique<MaybeUnusedConsumer>(&CI.getASTContext());
    }

    bool ParseArgs(
        const CompilerInstance&,
        const std::vector<std::string>&) override {
        return true;
    }

    ActionType getActionType() override {
        return AddBeforeMainAction;
    }
};

} // namespace

static FrontendPluginRegistry::Add<MaybeUnusedAction>
X("maybe-unused", "mark unused variables with [[maybe_unused]]");
