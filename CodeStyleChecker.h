//==============================================================================
// FILE:
//    CodeStyleChecker.h
//
// DESCRIPTION:
//    Declares the CodeStyleChecker visitor
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_CSC_H
#define CLANG_TUTOR_CSC_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class CodeStyleCheckerVisitor
	: public clang::RecursiveASTVisitor<CodeStyleCheckerVisitor>
{
public:
	explicit CodeStyleCheckerVisitor(clang::ASTContext *Ctx) : Ctx(Ctx) {}
	bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);
	bool VisitFunctionDecl(clang::FunctionDecl *Decl);
	bool VisitVarDecl(clang::VarDecl *Decl);
	bool VisitFieldDecl(clang::FieldDecl *Decl);

    bool VisitStringLiteral(clang::StringLiteral *SL);

private:
	clang::ASTContext *Ctx;

	void checkNameStartsWithLowerCase(clang::NamedDecl *Decl);

    void check_rule_1(clang::StringLiteral *SL);
    void check_rule_3_6(clang::CXXRecordDecl *SL);
};

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeStyleCheckerASTConsumer : public clang::ASTConsumer
{
public:
	explicit CodeStyleCheckerASTConsumer(
		clang::ASTContext *Context,
		bool MainFileOnly,
		clang::SourceManager &SM)
		: Visitor(Context), SM(SM), MainTUOnly(MainFileOnly) {}

	void HandleTranslationUnit(clang::ASTContext &Ctx)
	{
		if (!MainTUOnly)
		{
			Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
		}
		else
		{
			// Only visit declarations declared in the input TU
			auto Decls = Ctx.getTranslationUnitDecl()->decls();
			for (auto &Decl : Decls)
			{
				// Ignore declarations out of the main translation unit.
				//
				// SourceManager::isInMainFile method takes into account locations
				// expansion like macro expansion scenario and checks expansion
				// location instead if spelling location if required.
				if (!SM.isInMainFile(Decl->getLocation()))
				{
					continue;
				}
				Visitor.TraverseDecl(Decl);
			}
		}
	}

private:
	CodeStyleCheckerVisitor Visitor;
	clang::SourceManager &SM;
	// Should this plugin be only run on the main translation unit?
	bool MainTUOnly = true;
};

#endif