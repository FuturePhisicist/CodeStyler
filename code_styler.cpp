#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"
#include <regex>

using namespace clang;

namespace {

// Посетитель AST, который ищет нарушения код-стиля
class StyleCheckerVisitor : public RecursiveASTVisitor<StyleCheckerVisitor> {
public:
    explicit StyleCheckerVisitor(ASTContext &Context)
        : Context(Context), SM(Context.getSourceManager()) {}

    bool VisitVarDecl(VarDecl *VD) {
        // // Проверяем, что текущий файл - не системный заголовок
        // if (!SM.isInMainFile(VD->getLocation()))
        //     return true;

        // Правило 3.2: Использование английских слов в именах переменных
        std::string VarName = VD->getNameAsString();
        if (std::regex_search(VarName, std::regex("[А-Яа-я]"))) {
            DiagnosticsEngine &Diag = Context.getDiagnostics();
            unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                   "variable name contains non-English characters [CMC-OS]");
            Diag.Report(VD->getLocation(), DiagID);
        }

        // Правило 3.3: Константы должны быть полностью в верхнем регистре
        if (VD->isConstexpr() || VD->isConstexpr()) {
            if (!std::all_of(VarName.begin(), VarName.end(), ::isupper)) {
                DiagnosticsEngine &Diag = Context.getDiagnostics();
                unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "constant name should be in UPPER_CASE [CMC-OS]");
                Diag.Report(VD->getLocation(), DiagID);
            }
        }

        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *FD) {
        // // Проверяем, что текущий файл - не системный заголовок
        // if (!SM.isInMainFile(FD->getLocation()))
        //     return true;

        // Правило 3.2: Имена функций должны использовать только английские слова
        std::string FuncName = FD->getNameAsString();
        if (std::regex_search(FuncName, std::regex("[А-Яа-я]"))) {
            DiagnosticsEngine &Diag = Context.getDiagnostics();
            unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                   "function name contains non-English characters [CMC-OS]");
            Diag.Report(FD->getLocation(), DiagID);
        }

        // Правило 5.7: Проверка наличия return в main
        if (FD->isMain()) {
            bool HasReturn = false;
            for (const Stmt *S : FD->getBody()->children()) {
                if (isa<ReturnStmt>(S)) {
                    HasReturn = true;
                    break;
                }
            }
            if (!HasReturn) {
                DiagnosticsEngine &Diag = Context.getDiagnostics();
                unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "function 'main' must end with 'return 0;' statement [CMC-OS]");
                Diag.Report(FD->getLocation(), DiagID);
            }
        }

        return true;
    }

    bool VisitCallExpr(CallExpr *CE) {
        // // Проверяем, что текущий файл - не системный заголовок
        // if (!SM.isInMainFile(CE->getExprLoc()))
        //     return true;

        // Правило 5.8: Запрещены функции gets, strcpy, sprintf
        FunctionDecl *FD = CE->getDirectCallee();
        if (!FD)
            return true;

        std::string FuncName = FD->getNameInfo().getName().getAsString();
        if (FuncName == "gets" || FuncName == "strcpy" || FuncName == "sprintf") {
            DiagnosticsEngine &Diag = Context.getDiagnostics();
            unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                   "function '%0' is forbidden due to buffer overflow risks [CMC-OS]");
            Diag.Report(CE->getExprLoc(), DiagID) << FuncName;
        }

        return true;
    }

    bool VisitStringLiteral(StringLiteral *SL) {
        // // Проверяем, что текущий файл - не системный заголовок
        // if (!SM.isInMainFile(SL->getBeginLoc()))
        //     return true;

        StringRef Str = SL->getString();

        for (size_t i = 0; i < Str.size(); ++i) {
            char c = Str[i];

            if ((c < 32 && c != 10 && c != 13) || c == 127) {
                DiagnosticsEngine &Diag = Context.getDiagnostics();
                unsigned DiagID;

                if (c == '\t')
                {
                    DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "string literal contains '\\t' (R1.2) [CMC-OS]");
                }
                else
                {
                    DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "string literal contains invalid character (R1.1) [CMC-OS]");
                }
                
                Diag.Report(SL->getBeginLoc(), DiagID);
            }
        }

        return true;
    }

private:
    ASTContext &Context;
    const SourceManager &SM;
};

// Передний конец плагина, который инициализирует проверку
class StyleCheckerConsumer : public ASTConsumer {
public:
    explicit StyleCheckerConsumer(ASTContext &Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    StyleCheckerVisitor Visitor;
};

class StyleCheckerAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   llvm::StringRef) override {
        return std::make_unique<StyleCheckerConsumer>(CI.getASTContext());
    }

    bool ParseArgs(const CompilerInstance &CI,
                   const std::vector<std::string> &args) override {
        return true;
    }

    // Можно добавить дополнительные аргументы для плагина, если нужно
};

} // namespace

// Регистрация плагина
static FrontendPluginRegistry::Add<StyleCheckerAction>
X("style-checker", "Check code style according to custom rules");
