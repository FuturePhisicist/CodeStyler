//==============================================================================
// FILE:
//    CodeStyleChecker.cpp
//
// DESCRIPTION:
//    Checks whether function, variable and type names follow the LLVM's coding
//    style guide. If not, issue a warning and generate a FixIt hint. The
//    following items are exempt from the above rules and are ignored:
//      * anonymous fields in classes and structs
//      * anonymous unions
//      * anonymous function parameters
//      * conversion operators
//    These exemptions are further documented in the source code below.
//
//    This plugin is complete in the sense that it successfully processes
//    vector.h from STL. Also, note that it implements only a small subset of
//    LLVM's coding guidelines.
//
//    By default this plugin will only run on the main translation unit. Use
//    `-main-tu-only=false` to make it run on e.g. included header files too.
//
// USAGE:
//    1. As a loadable Clang plugin:
//    Main TU only:
//      * clang -cc1 -load <BUILD_DIR>/lib/libCodeStyleChecker.dylib -plugin '\'
//        CSC test/CodeStyleCheckerVector.cpp
//    All TUs (the main file and the #includ-ed header files)
//      * clang -cc1 -load <BUILD_DIR>/lib/libCodeStyleChecker.dylib '\'
//        -plugin CSC -plugin-arg-CSC -main-tu-only=false '\'
//        test/CodeStyleCheckerVector.cpp
//    2. As a standalone tool:
//        <BUILD_DIR>/bin/ct-code-style-checker '\'
//        test/ct-code-style-checker-basic.cpp
//
// License: The Unlicense
//==============================================================================
#include "CodeStyleChecker.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

//-----------------------------------------------------------------------------
// CodeStyleCheckerVisitor implementation
//-----------------------------------------------------------------------------
bool CodeStyleCheckerVisitor::VisitCXXRecordDecl(CXXRecordDecl *Decl)
{
	// Skip anonymous records, e.g. unions:
	//    * https://en.cppreference.com/w/cpp/language/union
	if (0 == Decl->getNameAsString().size())
	{
		return true;
	}

	check_rule_3_6(Decl);
	return true;
}

bool CodeStyleCheckerVisitor::VisitFunctionDecl(FunctionDecl *Decl)
{
	// Skip user-defined conversion operators/functions:
	//    * https://en.cppreference.com/w/cpp/language/cast_operator
	if (isa<CXXConversionDecl>(Decl))
	{
		return true;
	}

	checkNameStartsWithLowerCase(Decl);
	// checkNoUnderscoreInName(Decl);
	return true;
}

bool CodeStyleCheckerVisitor::VisitVarDecl(VarDecl *Decl)
{
	// Skip anonymous function parameter declarations
	if (isa<ParmVarDecl>(Decl) && (0 == Decl->getNameAsString().size()))
	{
		return true;
	}

	// checkNameStartsWithUpperCase(Decl);
	// checkNoUnderscoreInName(Decl);
	return true;
}

bool CodeStyleCheckerVisitor::VisitFieldDecl(FieldDecl *Decl)
{
	// Skip anonymous bit-fields:
	//  * https://en.cppreference.com/w/c/language/bit_field
	if (0 == Decl->getNameAsString().size())
	{
		return true;
	}

	// checkNameStartsWithUpperCase(Decl);
	// checkNoUnderscoreInName(Decl);

	return true;
}

bool CodeStyleCheckerVisitor::VisitStringLiteral(StringLiteral *SL)
{
	check_rule_1_1(SL);
	check_rule_1_2(SL);

	return true;
}

void CodeStyleCheckerVisitor::checkNameStartsWithLowerCase(NamedDecl *Decl)
{
	auto Name = Decl->getNameAsString();
	char FirstChar = Name[0];

	// The actual check
	if (isLowercase(FirstChar))
	{
		return;
	}

	// Construct the hint
	std::string Hint = Name;
	Hint[0] = toLowercase(FirstChar);
	FixItHint FixItHint = FixItHint::CreateReplacement(
		SourceRange(
			Decl->getLocation(),
			Decl->getLocation().getLocWithOffset(Name.size())),
			Hint);

	// Construct the diagnostic
	DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
	unsigned DiagID = DiagEngine.getCustomDiagID(
		DiagnosticsEngine::Warning,
		"Function names should start with lower-case letter");
	DiagEngine.Report(Decl->getLocation(), DiagID) << FixItHint;
}

void CodeStyleCheckerVisitor::check_rule_1_1(StringLiteral *SL)
{
	StringRef Str = SL->getString();

	for (size_t i = 0; i < Str.size(); ++i) {
		char c = Str[i];

		if ((c < 32 && c != 10 && c != 13 && c != '\t') || c == 127) {
			DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
			unsigned DiagID;

			// Construct the hint
			// FixItHint FixItHint = FixItHint::CreateRemoval(
			// 	SourceRange(
			// 		SL->getBeginLoc().getLocWithOffset(i + 1),
			// 		SL->getBeginLoc().getLocWithOffset(i + 1)));

			std::string Hint = std::string(Str);

			const auto invalid_char_pos = Hint.begin() + i;

			Hint.erase(invalid_char_pos, Hint.end());

			FixItHint FixItHint = FixItHint::CreateReplacement(
				SourceRange(SL->getBeginLoc(), SL->getEndLoc()),
				Hint);
			// FixItHint FixItHint = FixItHint::CreateRemoval(
			// 	SourceRange(SL->getBeginLoc().getLocWithOffset(1), SL->getBeginLoc().getLocWithOffset(i + 1)));

			DiagID = DiagEngine.getCustomDiagID(
				DiagnosticsEngine::Warning,
				"string literal contains invalid character (R1.1) [CMC-OS]");
			
			// DiagEngine.Report(SL->getBeginLoc(), DiagID);
			DiagEngine.Report(SL->getBeginLoc(), DiagID).AddFixItHint(FixItHint);
		}
	}
}

void CodeStyleCheckerVisitor::check_rule_1_2(StringLiteral *SL)
{
	StringRef Str = SL->getString();

	for (size_t i = 0; i < Str.size(); ++i) {
		char c = Str[i];

		if (c == '\t') {
			DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
			unsigned DiagID;

			// Construct the hint
			// FixItHint FixItHint = FixItHint::CreateRemoval(
			// 	SourceRange(
			// 		SL->getBeginLoc().getLocWithOffset(i + 1),
			// 		SL->getBeginLoc().getLocWithOffset(i + 1)));

			DiagID = DiagEngine.getCustomDiagID(
				DiagnosticsEngine::Warning,
				"string literal contains '\\t' (R1.2) [CMC-OS]");
			
			DiagEngine.Report(SL->getBeginLoc(), DiagID);
		}
	}
}

void CodeStyleCheckerVisitor::check_rule_3_6(CXXRecordDecl *Decl)
{
	auto Name = Decl->getNameAsString();

	std::string Hint = Name;


	bool hasChanged = false;
	if (!isUppercase(Name[0]))
	{
		Hint[0] = toUppercase(Hint[0]);
		hasChanged = true;
	}

	size_t underscorePos = Name.find('_');
	if (underscorePos != StringRef::npos)
	{
		hasChanged = true;
		for (int i = 0; i < Hint.size() - 1; ++i)
		{
			if (Hint[i] == '_')
			{
				Hint[i + 1] = toUppercase(Hint[i + 1]);
			}
		}
	}

	if (hasChanged)
	{
		auto end_pos = std::remove(Hint.begin(), Hint.end(), '_');
		Hint.erase(end_pos, Hint.end());

		FixItHint FixItHint = FixItHint::CreateReplacement(
			SourceRange(Decl->getLocation(),
			Decl->getLocation().getLocWithOffset(Name.size())),
			Hint);

		DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
		unsigned DiagID = DiagEngine.getCustomDiagID(
			DiagnosticsEngine::Warning,
			"type name must be in UpperCamelCase (`_` is not allowed) (R3.6) [CMC-OS]");

		SourceLocation UnderscoreLoc =
			Decl->getLocation().getLocWithOffset(underscorePos);

		DiagEngine.Report(UnderscoreLoc, DiagID).AddFixItHint(FixItHint);
	}
}

//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class CSCASTAction : public PluginASTAction
{
public:
	std::unique_ptr<ASTConsumer>
	CreateASTConsumer(
		CompilerInstance &Compiler,
		llvm::StringRef InFile) override
	{
		return std::make_unique<CodeStyleCheckerASTConsumer>(
			&Compiler.getASTContext(),
			MainTuOnly,
			Compiler.getSourceManager());
	}

	bool ParseArgs(
		const CompilerInstance &CI,
		const std::vector<std::string> &Args) override
	{
		for (StringRef Arg : Args)
		{
			if (Arg.starts_with("-main-tu-only="))
			{
				MainTuOnly =
				Arg.substr(strlen("-main-tu-only=")).equals_insensitive("true");
			}
			else if (Arg.starts_with("-help"))
			{
				PrintHelp(llvm::errs());
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	void PrintHelp(llvm::raw_ostream &ros)
	{
		ros << "Help for CodeStyleChecker plugin goes here\n";
	}

private:
	bool MainTuOnly = true;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static clang::FrontendPluginRegistry::Add<CSCASTAction>
	X(/*Name=*/"CSC",
	/*Description=*/"Checks whether class, variable and function names "
		"adhere to LLVM's guidelines");