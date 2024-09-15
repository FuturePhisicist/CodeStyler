#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include <regex>

using namespace clang;

namespace {

// Посетитель AST, который ищет нарушения код-стиля
class StyleCheckerVisitor : public RecursiveASTVisitor<StyleCheckerVisitor> {
public:
    explicit StyleCheckerVisitor(ASTContext &Context)
        : Context(Context) {}

    // Проверка управляющих символов
    void CheckControlCharacters(const SourceManager &SM, const FileID &FID) {
        const FileEntry *FE = SM.getFileEntryForID(FID);
        if (!FE)
            return;

        // Получаем путь к файлу
        llvm::StringRef FilePath = FE->getName();
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> BufOrErr =
            llvm::MemoryBuffer::getFile(FilePath);

        if (std::error_code EC = BufOrErr.getError()) {
            llvm::errs() << "Error reading file: " << EC.message() << "\n";
            return;
        }

        std::unique_ptr<llvm::MemoryBuffer> Buf = std::move(BufOrErr.get());

        StringRef FileContents = Buf->getBuffer();
        for (size_t i = 0; i < FileContents.size(); ++i) {
            unsigned char c = FileContents[i];
            // Проверяем управляющие символы от 0 до 31 и 127 (кроме CR и LF)
            if ((c < 32 && c != 10 && c != 13) || c == 127) {
                FullSourceLoc FullLocation(SM.getLocForStartOfFile(FID), SM);
                unsigned Line = SM.getLineNumber(FID, i + 1);
                unsigned Column = SM.getColumnNumber(FID, i + 1);

                DiagnosticsEngine &Diag = Context.getDiagnostics();
                unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "File contains invalid control character [CMC-OS]");
                Diag.Report(SM.getLocForStartOfFile(FID), DiagID)
                    << Line << Column;
            }
        }
    }

    bool VisitVarDecl(VarDecl *VD) {
        // Правило 3.2: Использование английских слов в именах переменных
        std::string VarName = VD->getNameAsString();
        if (std::regex_search(VarName, std::regex("[А-Яа-я]"))) {
            DiagnosticsEngine &Diag = Context.getDiagnostics();
            unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                   "Variable name contains non-English characters [CMC-OS]");
            Diag.Report(VD->getLocation(), DiagID);
        }

        // Правило 3.3: Константы должны быть полностью в верхнем регистре
        if (VD->isConstexpr() || VD->isConstexpr()) {
            if (!std::all_of(VarName.begin(), VarName.end(), ::isupper)) {
                DiagnosticsEngine &Diag = Context.getDiagnostics();
                unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                       "Constant name should be in UPPER_CASE [CMC-OS]");
                Diag.Report(VD->getLocation(), DiagID);
            }
        }

        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *FD) {
        // Правило 3.2: Имена функций должны использовать только английские слова
        std::string FuncName = FD->getNameAsString();
        if (std::regex_search(FuncName, std::regex("[А-Яа-я]"))) {
            DiagnosticsEngine &Diag = Context.getDiagnostics();
            unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                   "Function name contains non-English characters [CMC-OS]");
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
                                                       "Function 'main' must end with 'return 0;' statement [CMC-OS]");
                Diag.Report(FD->getLocation(), DiagID);
            }
        }

        return true;
    }

    bool VisitCallExpr(CallExpr *CE) {
        // Правило 5.8: Запрещены функции gets, strcpy, sprintf
        FunctionDecl *FD = CE->getDirectCallee();
        if (!FD)
            return true;

        std::string FuncName = FD->getNameInfo().getName().getAsString();
        if (FuncName == "gets" || FuncName == "strcpy" || FuncName == "sprintf") {
            DiagnosticsEngine &Diag = Context.getDiagnostics();
            unsigned DiagID = Diag.getCustomDiagID(DiagnosticsEngine::Warning,
                                                   "Function '%0' is forbidden due to buffer overflow risks");
            Diag.Report(CE->getExprLoc(), DiagID) << FuncName;
        }

        return true;
    }

    void CheckFileForControlCharacters(const SourceManager &SM) {
        FileID FID = SM.getMainFileID();
        CheckControlCharacters(SM, FID);
    }

private:
    ASTContext &Context;
};

// Передний конец плагина, который инициализирует проверку
class StyleCheckerConsumer : public ASTConsumer {
public:
    explicit StyleCheckerConsumer(ASTContext &Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());

        // Проверяем наличие управляющих символов
        Visitor.CheckFileForControlCharacters(Context.getSourceManager());
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
