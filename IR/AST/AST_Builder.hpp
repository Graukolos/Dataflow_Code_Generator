#pragma once

#include "AST.hpp"
#include "Lexer/Lexer.hpp"

namespace AST {

	class AST_Builder {
	private:
		AST_Root* ast;

		Unit* current_unit;
		Actor* current_actor;
		Action* current_action;
		Procedure* current_procedure;
		Function* current_function;
		ActorParameter* current_actorparam;
		Action_Priority* current_prio;
		Schedule_FSM* current_schedulefsm;
		Schedule_FSM_State* current_fsmstate;

		std::vector<AST::Statement*> statement_stack;
		Statement* current_statement;

		/* I think half of this could be organized as stack of base expression */
		struct ExprContext {
			Expression* current_expression;
			Index* current_index;
			UnaryOperator* current_unaryop;
			Operand* current_operand;
			Generator* current_generator;
			ListComprehension* current_listcomprehension;
			Call* current_call;
			Operator* current_operator;
		};
		ExprContext current_expression_context;
		std::vector<ExprContext> expression_context_stack;


		Input_Pattern* current_inputpattern;
		Output_Expression* current_outputexpr;
		VarDefinition* current_vardef;
		Type* current_type;
		std::vector<Type*> type_stack;
		Parameter* current_param;
		Import* current_import;
		NativeFunction* current_nativefunc;
		NativeProcedure* current_nativeproc;
		Port* current_port;

		ID statement_identifier;

		bool outports = false;
		bool next_expr_braket = false;

		AST::ID idlist;

		enum ParserContext
		{
			File,
			Import,
			Actor,
			ActorParameter,
			PortDecl,
			Parameter,
			VarDefinition,
			Unit,
			Action,
			InitAction,
			Type,
			Statement,
			IfStatement,
			ElseStatement,
			CallStatement,
			BlockStatement,
			WhileStatement,
			ForeachStatement,
			AssignmentStatement,
			Index,
			UnaryOp,
			CallExpression,
			Operator,
			Literal,
			Expression,
			TernaryExpression,
			ListComprehension,
			Generator,
			Procedure,
			Function,
			NativeFunction,
			NativeProcedure,
			InputPattern,
			OutputExpression,
			Repeat,
			Guard,
			ScheduleFSM,
			StateTransition,
			PriorityInequality,
			IDList,
			Array
		};
		std::vector<ParserContext> context_stack;
		ParserContext context;

		void init_expression_context(ExprContext* e);

	public:

		AST_Builder();

		void add_token(Lexer::Token& t);

		AST_Root* get_ast(void);

		void start_Import(void);
		void end_Import(void);

		void start_Actor(void);
		void end_Actor(void);

		void start_ActorPar(void);
		void end_ActorPar(void);

		void start_PortDecl(void);
		void end_PortDecl(void);

		void start_Unit(void);
		void end_Unit(void);

		void start_Parameter(void);
		void end_Parameter(void);

		void start_VarDefinition(void);
		void end_VarDefinition(void);

		void start_Action(void);
		void end_Action(void);

		void start_InitAction(void);
		void end_InitAction(void);

		void start_Type(void);
		void end_Type(void);

		void start_Statement(void);
		void end_Statement(void);

		void start_IfStatement(void);
		void end_IfStatement(void);

		void start_ElseStatement(void);

		void start_CallStatement(void);
		void end_CallStatement(void);

		void start_BlockStatement(void);
		void end_BlockStatement(void);

		void start_WhileStatement(void);
		void end_WhileStatement(void);

		void start_ForeachStatement(void);
		void end_ForeachStatement(void);

		void start_AssignmentStatement(void);
		void end_AssignmentStatement(void);

		void start_Index(void);
		void end_Index(void);

		void start_CallExpression(void);
		void end_CallExpression(void);

		void start_Operator(void);
		void end_Operator(void);

		void start_Literal(void);
		void end_Literal(void);

		void start_Expression(void);
		void end_Expression(void);

		void start_TernaryExpression(void);
		void end_TernaryExpression(void);

		void start_UnaryOp(void);
		void end_UnaryOp(void);

		void start_ListComprehension(void);
		void end_ListComprehension(void);

		void start_Generator(void);
		void end_Generator(void);

		void start_Procedure(void);
		void end_Procedure(void);

		void start_Function(void);
		void end_Function(void);

		void start_NativeFunction(void);
		void end_NativeFunction(void);

		void start_NativeProcedure(void);
		void end_NativeProcedure(void);

		void start_InputPattern(void);
		void end_InputPattern(void);

		void start_OutputExpression(void);
		void end_OutputExpression(void);

		void start_Repeat(void);
		void end_Repeat(void);

		void start_Guard(void);
		void end_Guard(void);

		void start_scheduleFSM(void);
		void end_scheduleFSM(void);

		void start_StateTransition(void);
		void end_StateTransition(void);

		void start_PriorityInequality(void);
		void end_PriorityInequality(void);
	
		void start_IDList(void);
		void end_IDList(void);

		void start_Array(void);
		void end_Array(void);
	};

}