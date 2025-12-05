#pragma once

#include "common/include/Exceptions.hpp"
#include "Lexer/Lexer.hpp"
#include "IR/AST/AST.hpp"
#include "IR/AST/AST_Builder.hpp"

#include <vector>

namespace Parser {

	enum SymbolType
	{
		Nonterminal,
		Terminal
	};

	enum TerminalType {
		ID,
		String,
		Keyword,
		Number,
		Special
	};

	enum NonterminalType {
		Start,
		CALFile,
		Package,
		CALFileContent,
		Unit,
		UnitBody,
		UnitEnd,
		Actor,
		ActorEnd,
		ActorBody,
		IOSig,
		QualID,
		QualIDList,
		QualIDListElement,
		ActorPars,
		ActorPar,
		ActorParsList,
		PortDecl,
		PortDecls,
		PortDeclList,
		Imports,
		Import,
		/* Scheduling */
		ActionSchedule,
		ScheduleFSM,
		ScheduleFSMEnd,
		StateTransitionList,
		StateTransition,
		ActionTags,
		ActionTagList,
		PriorityOrder,
		PriorityInequality,
		PriorityInequalityList,
		ActionTagInequalityList,
		/* Types */
		Type,
		IntegerType,
		IntegerTypes,
		Size,
		ListType,
		FloatType,
		FloatTypes,
		/* Operations */
		BooleanOperator,
		ArithOperator,
		BitOperator,
		relOperator,
		unaryBooleanOperator,
		unaryBitOperator,
		unaryArithOperator,
		Op,
		unaryOp,
		/* Expressions */
		Expression,
		Expressions,
		ExpressionList,
		ConstCalc,
		SingleConstCalc,
		SingleConstCalcList,
		SingleExpression,
		SingleExpressionList,
		Negation,
		ExpressionLiteral,
		IfExpression,
		IfEnd,
		ListComprehension,
		ListComprehensionContent,
		ListComprehensionGenerator,
		ListComprehensionExprs,
		ListComprehensionExprsList,
		Generator,
		Generators,
		GeneratorList,
		CallExpr,
		CallParameters,
		Index,
		SingleExpressionAdd,
		/* Functions */
		FuncDecl,
		FuncDeclEnd,
		ProcDecl,
		ProcDeclEnd,
		NativeDecl,
		NativeDeclDes,
		NativeFuncDecl,
		NativeProcedureDecl,
		VarDecl,
		VarDecls,
		VarDeclList,
		ArrayDef,
		FormalPar,
		FormalPars,
		FormalParsList,
		/* Statement */
		Statement,
		IDStmt,
		AssignmentStmt,
		AssignmentExpr,
		CallStmt,
		BlockStmt,
		BlockVarStmt,
		IfStmt,
		ElseStmt,
		WhileStmt,
		WhileStmtEnd,
		ForeachStmt,
		ForeachGenerator,
		ForeachGeneratorList,
		ForeachStmtEnd,
		GeneratorType,
		Statements,
		ConstAsgn,
		Asgn,
		/* Action */
		Action,
		InitializationAction,
		InitializationActionEnd,
		ActionTag,
		ActionTagIDs,
		ActionEnd,
		ActionHead,
		IDs,
		IDList,
		GuardBlock,
		VarBlock,
		ActionName,
		StatementBlock,
		InputPattern,
		InputPatterns,
		InputPatternList,
		OutputExpression,
		OutputExpressions,
		OutputExpressionList,
		OutputExpressionContent,
		OutputExpressionContentList,
		RepeatClause,
		InitializerHead,
		/* Simplifications for AST Generation */
		IndexEnd,
		TypeEnd,
		GuardEnd,
		RepeatEnd,
		CallExpressionEnd,
		LiteralEnd,
		UnaryOpEnd,
		OpEnd
	};

	struct Symbol {
		std::string term_str; /* Only for terminals */
		SymbolType type;
		NonterminalType nonterm_type;
		TerminalType term_type;
	};

	class Parser_Class {
		Lexer::Lexer *lexer;

		std::vector<Symbol> keller;
		Lexer::Token current;

		AST::AST_Builder builder{};

	public:

		Parser_Class(Lexer::Lexer* l) : lexer{ l } { }

		AST::AST_Root* parse(void);

	};

	class Parser_Exception : public std::exception {
	public:
		virtual const char* what() const throw() = 0;
	};

	class Parser_Exception_Unexpected_Symbol : public Parser_Exception {
	public:
		Parser_Exception_Unexpected_Symbol(unsigned _l, unsigned _c, std::string _msg) : Parser_Exception{ }, line{ _l }, character{ _c }, msg{ _msg } {
			local = "Parser Error: Unexpected Symbol at line " + std::to_string(line) + " character " + std::to_string(character) + "\n";
			local.append(msg);
		};

		virtual const char* what() const throw()
		{
			return local.c_str();
		}

	private:
		std::string local;
		std::string msg;
		unsigned line;
		unsigned character;
	};

}