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

	check_rule_3_4(Decl);

	return true;
}

bool CodeStyleCheckerVisitor::VisitVarDecl(VarDecl *Decl)
{
	// Skip anonymous function parameter declarations
	if (isa<ParmVarDecl>(Decl) && (0 == Decl->getNameAsString().size()))
	{
		return true;
	}

	// if (constexpr Decl || const Decl)
	if (Decl->isConstexpr() || Decl->getType().isConstQualified())
	{
		check_rule_3_3(Decl);

		return true;
	}

	check_rule_3_4(Decl);

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

	return true;
}

bool CodeStyleCheckerVisitor::VisitStringLiteral(StringLiteral *SL)
{
	check_rule_1(SL);

	return true;
}

void CodeStyleCheckerVisitor::check_rule_1(StringLiteral *SL)
{
	StringRef Str = SL->getString();

	bool hasChanged = false;
	std::string Hint;

	for (size_t i = 0; i < Str.size(); ++i) {
		char c = Str[i];

		// printf("%d\n", c);

		if ((c < 32 && c != 10 && c != 13) || c == 127) {
			hasChanged = true;
		}
		else
		{
			Hint.push_back(c);
		}
	}

	Hint = "\"" + Hint + "\"";

	if (hasChanged)
	{
		DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();

		FixItHint FixItHint = FixItHint::CreateReplacement(
			SourceRange(SL->getBeginLoc(), SL->getEndLoc()),
			Hint);

		unsigned DiagID = DiagEngine.getCustomDiagID(
			DiagnosticsEngine::Warning,
			"string literal contains invalid characters (including '\\t') (R1.1, R1.2) [CMC-OS]");
		
		// DiagEngine.Report(SL->getBeginLoc(), DiagID);
		DiagEngine.Report(SL->getBeginLoc(), DiagID).AddFixItHint(FixItHint);
	}
}

void CodeStyleCheckerVisitor::check_rule_3_3(NamedDecl *Decl)
{
	auto Name = Decl->getNameAsString();

	std::string Hint = Name;
	std::transform(Hint.begin(), Hint.end(), Hint.begin(), ::toupper);;

	if (Hint != Name)
	{
		FixItHint FixItHint = FixItHint::CreateReplacement(
			SourceRange(Decl->getLocation(),
			Decl->getLocation().getLocWithOffset(Name.size() - 1)),
			Hint);

		DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
		unsigned DiagID = DiagEngine.getCustomDiagID(
			DiagnosticsEngine::Warning,
			"consts, constexprs and enums name must be in SCREAMING_SNAKE_CASE (R3.3) [CMC-OS]");

		size_t firstLowerCaseChar = 0;
		for (size_t i = 0; i < Name.size(); ++i) {
	        if (islower(Name[i])) {
	            firstLowerCaseChar = i;
	            break;
	        }
	    }

		DiagEngine.Report(Decl->getLocation().getLocWithOffset(firstLowerCaseChar), DiagID).AddFixItHint(FixItHint);
	}
}

void CodeStyleCheckerVisitor::check_rule_3_4(NamedDecl *Decl)
{
	auto Name = Decl->getNameAsString();

	std::string Hint = Name;
	std::transform(Hint.begin(), Hint.end(), Hint.begin(), ::tolower);;

	if (Hint != Name)
	{
		FixItHint FixItHint = FixItHint::CreateReplacement(
			SourceRange(Decl->getLocation(),
			Decl->getLocation().getLocWithOffset(Name.size() - 1)),
			Hint);

		DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
		unsigned DiagID = DiagEngine.getCustomDiagID(
			DiagnosticsEngine::Warning,
			"variable, function and label name must be in snake_case (R3.4) [CMC-OS]");

		size_t firstUpperCaseChar = 0;
		for (size_t i = 0; i < Name.size(); ++i) {
	        if (isupper(Name[i])) {
	            firstUpperCaseChar = i;
	            break;
	        }
	    }

		DiagEngine.Report(Decl->getLocation().getLocWithOffset(firstUpperCaseChar), DiagID).AddFixItHint(FixItHint);
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