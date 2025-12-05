#include "AST_Builder.hpp"

#include <assert.h>
#include <iostream>

void AST::AST_Builder::init_expression_context(ExprContext* e)
{
	e->current_call = nullptr;
	e->current_expression = nullptr;
	e->current_generator = nullptr;
	e->current_index = nullptr;
	e->current_listcomprehension = nullptr;
	e->current_operand = nullptr;
	e->current_operator = nullptr;
	e->current_unaryop = nullptr;
}

AST::AST_Builder::AST_Builder()
{
	this->ast = new AST_Root{};
	this->context = File;
	this->current_action = nullptr;
	this->current_actor = nullptr;
	this->current_statement = nullptr;
	this->current_unit = nullptr;
	this->current_function = nullptr;
	this->current_procedure = nullptr;
	this->current_actorparam = nullptr;
	this->current_inputpattern = nullptr;
	this->current_outputexpr = nullptr;
	this->current_vardef = nullptr;
	this->current_type = nullptr;
	this->current_param = nullptr;
	this->current_schedulefsm = nullptr;
	this->current_fsmstate = nullptr;
	this->current_import = nullptr;
	this->current_prio = nullptr;
	this->current_port = nullptr;
	this->current_nativefunc = nullptr;
	this->current_nativeproc = nullptr;

	init_expression_context(&current_expression_context);
}

void AST::AST_Builder::add_token(Lexer::Token& t)
{
	//std::cout << "Retired " << t.str << " in context " << context << " L: " <<t.line << " C: " << t.character << std::endl;

	if (t.str == ",") {
		/* never relevant for AST, skip */
		return;
	}

	if (context == File) {
		/* nothing to do in this scope */
	}
	else if (context == IDList) {
		if (idlist.name.empty()) {
			idlist.line = t.line;
			idlist.character = t.character;
		}
		idlist.name.append(t.str);
	}
	else if (context == Import) {
		if (t.type == Lexer::Identifier) {
			assert(current_import != nullptr);
			current_import->symbol.name = t.str;
		}
		/* Nothing to do, only drop keyword etc. */
	}
	else if (context == Actor) {
		if (t.str == "==>") {
			outports = true;
		}
		if (t.type == Lexer::Identifier) {
			assert(current_actor != nullptr);
			current_actor->name.name = t.str;
			current_actor->name.line = t.line;
			current_actor->name.character = t.character;
		}
		/* drop some keywords etc. */
	}
	else if (context == ActorParameter) {
		if (t.type == Lexer::Identifier) {
			assert(current_actorparam != nullptr);
			current_actorparam->name.name = t.str;
			current_actorparam->name.line = t.line;
			current_actorparam->name.character = t.character;
		}
	}
	else if (context == PortDecl) {
		assert(current_port != nullptr);
		if (t.type == Lexer::Identifier) {
			current_port->name.name = t.str;
			current_port->name.line = t.line;
			current_port->name.character = t.character;
		}
	}
	else if (context == Parameter) {
		assert(current_param != nullptr);
		if (t.type == Lexer::Identifier) {
			current_param->name.name = t.str;
			current_param->name.line = t.line;
			current_param->name.character = t.character;
		}
	}
	else if (context == VarDefinition) {
		assert(current_vardef != nullptr);
		if (t.type == Lexer::Identifier) {
			current_vardef->name.name = t.str;
			current_vardef->name.line = t.line;
			current_vardef->name.character = t.character;
		}
		else if (t.str == ":") {
			current_vardef->constassign = false;
		}
	}
	else if (context == Unit) {
		assert(current_unit != nullptr);
		if (t.type == Lexer::Identifier) {
			current_unit->name.name = t.str;
			current_unit->name.line = t.line;
			current_unit->name.character = t.character;
		}
	}
	else if (context == Action) {
		/* nothing to do, drop keywords etc. */
	}
	else if (context == InitAction) {
		/* nothing to do, drop keyword etc. */
	}
	else if (context == Type) {
		assert(current_type != nullptr);
		if ((t.type == Lexer::Keyword) &&
			((t.str == "list") || (t.str == "uint") || (t.str == "int") ||
				(t.str == "float") || (t.str == "half") || (t.str == "String") || (t.str == "bool")))
		{
			current_type->type.name = t.str;
			current_type->type.line = t.line;
			current_type->type.character = t.character;
		}
	}
	else if (context == Statement) {
		assert(t.type == Lexer::Identifier);
		statement_identifier.name = t.str;
		statement_identifier.line = t.line;
		statement_identifier.character = t.character;
	}
	else if (context == IfStatement) {
		/* Nothing to do, only drop keywords etc. */
	}
	else if (context == ElseStatement) {
		/* Nothing to do, only drop keywords etc. */
	}
	else if (context == CallStatement) {
		/* nothing to do, only drop keyword etc. */
		if (t.str == ";") {
			end_CallStatement();
		}
	}
	else if (context == BlockStatement) {
		/* nothing to do, only drop keyword etc. */
		if (t.str == "end") {
			end_BlockStatement();
		}
	}
	else if (context == WhileStatement) {
		/* nothing to do, only drop keywords etc. */
	}
	else if (context == ForeachStatement) {
		/* nothing to do, only drop keywords etc. */
	}
	else if (context == AssignmentStatement) {
		assert(current_statement != nullptr);
		auto a = dynamic_cast<AST::AssignmentStatement*>(current_statement);
		assert(a != nullptr);
		if (t.str == ":") {
			a->constasgn = false;
		}
		if (t.str == ";") {
			end_AssignmentStatement();
		}
	}
	else if (context == Index) {
		/* nothing to do, we just drop the two brakets */
	}
	else if (context == TernaryExpression) {
		/* nothing to do, just drop the keywords */
	}
	else if (context == CallExpression) {
		/* Nothing to do, simply skip keyword etc. */
	}
	else if (context == Operator) {
		assert(current_expression_context.current_operator != nullptr);
		if (current_expression_context.current_operator->ops.empty()) {
			current_expression_context.current_operator->character = t.character;
			current_expression_context.current_operator->line = t.line;
		}
		current_expression_context.current_operator->ops.append(t.str);
	}
	else if (context == Literal) {
		assert(current_expression_context.current_operand != nullptr);
		auto l = dynamic_cast<AST::Literal*>(current_expression_context.current_operand);
		assert(l != nullptr);
		if (l->literal.empty()) {
			l->line = t.line;
			l->character = t.character;
		}
		if (t.str == "-") {
			l->negation = true;
		}
		else {
			l->literal.append(t.str);
		}
	}
	else if (context == Expression) {
		assert(next_expr_braket == false);
		assert(current_expression_context.current_expression != nullptr);
		if (t.str == "(") {
			next_expr_braket = true;
			return;
		}
		if (t.type == Lexer::Identifier) {
			auto i = new AST::Identifier();
			current_expression_context.current_operand = i;
			if (current_expression_context.current_unaryop != nullptr) {
				i->unary_left = current_expression_context.current_unaryop;
				current_expression_context.current_unaryop = nullptr;
			}
			i->character = t.character;
			i->line = t.line;
			i->identifier = t.str;
		}
	}
	else if (context == ListComprehension) {
		/* nothing to do, simply skip the keywords etc. */
	}
	else if (context == Generator) {
		assert(current_expression_context.current_generator != nullptr);
		if (t.type == Lexer::Identifier) {
			current_expression_context.current_generator->identifier.name = t.str;
			current_expression_context.current_generator->identifier.line = t.line;
			current_expression_context.current_generator->identifier.character = t.character;
		}
	}
	else if (context == Procedure) {
		assert(current_procedure != nullptr);
		if (t.type == Lexer::Identifier) {
			current_procedure->name.name = t.str;
			current_procedure->name.line = t.line;
			current_procedure->name.character = t.character;
		}
	}
	else if (context == Function) {
		assert(current_function != nullptr);
		if (t.type == Lexer::Identifier) {
			current_function->name.name = t.str;
			current_function->name.line = t.line;
			current_function->name.character = t.character;
		}
	}
	else if (context == UnaryOp) {
		assert(current_expression_context.current_unaryop != nullptr);
		if (current_expression_context.current_unaryop->ops.empty()) {
			current_expression_context.current_unaryop->line = t.line;
			current_expression_context.current_unaryop->character = t.character;
		}
		current_expression_context.current_unaryop->ops.append(t.str);
	}
	else if (context == NativeFunction) {
		assert(current_nativefunc != nullptr);
		if (t.type == Lexer::Identifier) {
			current_nativefunc->name.name = t.str;
			current_nativefunc->name.line = t.line;
			current_nativefunc->name.character = t.character;
		}
	}
	else if (context == NativeProcedure) {
		assert(current_nativeproc != nullptr);
		if (t.type == Lexer::Identifier) {
			current_nativeproc->name.name = t.str;
			current_nativeproc->name.line = t.line;
			current_nativeproc->name.character = t.character;
		}
	}
	else if (context == InputPattern) {
		assert(current_inputpattern != nullptr);
		if (t.type == Lexer::Identifier) {
			if (current_inputpattern->port.name.empty()) {
				current_inputpattern->port.name = t.str;
				current_inputpattern->port.line = t.line;
				current_inputpattern->port.character = t.character;
			}
			else {
				ID x;
				x.character = t.character;
				x.line = t.line;
				x.name = t.str;
				current_inputpattern->IDs.push_back(x);
			}
		}
	}
	else if (context == OutputExpression) {
		assert(current_outputexpr != nullptr);
		if (t.type == Lexer::Identifier) {
			current_outputexpr->port.name = t.str;
			current_outputexpr->port.line = t.line;
			current_outputexpr->port.character = t.character;
		}
	}
	else if (context == Repeat) {
		/* nothing to do, just drop keywords */
	}
	else if (context == Guard) {
		/* nothing to do, just drop keywords */
	}
	else if (context == ScheduleFSM) {
		if (t.type == Lexer::Identifier) {
			assert(current_schedulefsm != nullptr);
			assert(current_schedulefsm->inital_state.name.empty());
			current_schedulefsm->inital_state.name = t.str;
			current_schedulefsm->inital_state.line = t.line;
			current_schedulefsm->inital_state.character = t.character;
		}
	}
	else if (context == StateTransition) {
		assert(current_fsmstate != nullptr);
		if (t.type == Lexer::Identifier) {
			if (current_fsmstate->state.name.empty()) {
				current_fsmstate->state.name = t.str;
				current_fsmstate->state.line = t.line;
				current_fsmstate->state.character = t.character;
			}
			else {
				assert(current_fsmstate->next.name.empty());
				current_fsmstate->next.name = t.str;
				current_fsmstate->next.line = t.line;
				current_fsmstate->next.character = t.character;
			}
		}
	}
	else if (context == PriorityInequality) {
		/* nothing to do, only drop keywords etc. */
	}
	else if (context == Array) {
		if ((t.type == Lexer::Delimiter) && (t.str == "]")) {
			end_Array();
		}
	}
	else {
		std::cout << "Unknown context!" << std::endl;
		assert(0);
	}
	//std::cout << "Context after retirement: " << context << std::endl;
}

AST::AST_Root* AST::AST_Builder::get_ast(void)
{
	return this->ast;
}

void AST::AST_Builder::start_Import(void)
{
	context_stack.push_back(context);
	context = Import;
	current_import = new AST::Import();
}

void AST::AST_Builder::end_Import(void)
{
	/* Callback might be called not in the Import context! */
	if (context == Import) {
		assert(current_import != nullptr);
		context = context_stack.back();
		context_stack.pop_back();
		ast->imports.push_back(current_import);
		current_import = nullptr;
		
		assert(context == File);
	}
}

void AST::AST_Builder::start_Actor(void)
{
	context_stack.push_back(context);
	context = Actor;
	current_actor = new AST::Actor();
}
void AST::AST_Builder::end_Actor(void)
{
	assert(context == Actor);
	assert(current_actor != nullptr);
	assert(ast != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	this->ast->actor = current_actor;
	current_actor = nullptr;

	assert(context == File);
}

void AST::AST_Builder::start_ActorPar(void)
{
	context_stack.push_back(context);
	context = ActorParameter;
	current_actorparam = new AST::ActorParameter();
}

void AST::AST_Builder::end_ActorPar(void)
{
	assert(context == ActorParameter);
	{
		assert(current_actorparam != nullptr);
		assert(current_actor != nullptr);
		context = context_stack.back();
		context_stack.pop_back();
		current_actor->parameters.push_back(current_actorparam);
		current_actorparam = nullptr;
		assert(context == Actor);
	}
}

void AST::AST_Builder::start_PortDecl(void)
{
	context_stack.push_back(context);
	context = PortDecl;
	current_port = new AST::Port();
}

void AST::AST_Builder::end_PortDecl(void)
{
	assert(context == PortDecl);
	{
		context = context_stack.back();
		context_stack.pop_back();
		assert(current_actor != nullptr);
		assert(current_port != nullptr);
		if (outports) {
			current_actor->outports.push_back(current_port);
		}
		else {
			current_actor->inports.push_back(current_port);
		}
		current_port = nullptr;
	}
}

void AST::AST_Builder::start_Unit(void)
{
	context_stack.push_back(context);
	context = Unit;
	current_unit = new AST::Unit();
}

void AST::AST_Builder::end_Unit(void)
{
	assert(context == Unit);
	assert(current_unit != nullptr);
	assert(ast != nullptr);

	context = context_stack.back();
	context_stack.pop_back();
	ast->unit = current_unit;
	current_unit = nullptr;

	assert(context == File);
}

void AST::AST_Builder::start_Parameter(void)
{
	context_stack.push_back(context);
	context = Parameter;
	current_param = new AST::Parameter();
}

void AST::AST_Builder::end_Parameter(void)
{
	assert(context == Parameter);
	{
		assert(current_param != nullptr);
		context = context_stack.back();
		context_stack.pop_back();

		if (context == Function) {
			assert(current_function != nullptr);
			current_function->parameters.push_back(current_param);
		}
		else if (context == NativeFunction) {
			assert(current_nativefunc != nullptr);
			current_nativefunc->parameters.push_back(current_param);
		}
		else if (context == NativeProcedure) {
			assert(current_nativeproc != nullptr);
			current_nativeproc->parameters.push_back(current_param);
		}
		else if (context == Procedure) {
			assert(current_procedure != nullptr);
			current_procedure->parameters.push_back(current_param);
		}
		else {
			assert(0);
		}
		current_param = nullptr;
	}
}

void AST::AST_Builder::start_VarDefinition(void)
{
	context_stack.push_back(context);
	context = VarDefinition;
	current_vardef = new AST::VarDefinition();
}

void AST::AST_Builder::end_VarDefinition(void)
{
	if (context == VarDefinition) {
		assert(current_vardef != nullptr);
		context = context_stack.back();
		context_stack.pop_back();

		if (current_vardef->assign == nullptr) {
			current_vardef->constassign = false;
		}

		if (context == Action) {
			assert(current_action != nullptr);
			current_action->vars.push_back(current_vardef);
		}
		else if (context == InitAction) {
			assert(current_action != nullptr);
			current_action->vars.push_back(current_vardef);
		}
		else if (context == BlockStatement) {
			AST::BlockStatement* b = dynamic_cast<AST::BlockStatement*>(current_statement);
			assert(b != nullptr);
			b->vars.push_back(current_vardef);
		}
		else if (context == WhileStatement) {
			AST::WhileStatement* w = dynamic_cast<AST::WhileStatement*>(current_statement);
			assert(w != nullptr);
			w->vars.push_back(current_vardef);
		}
		else if (context == ForeachStatement) {
			AST::ForeachStatement* f = dynamic_cast<AST::ForeachStatement*>(current_statement);
			assert(f != nullptr);
			f->vars.push_back(current_vardef);
		}
		else if (context == Procedure) {
			assert(current_procedure != nullptr);
			current_procedure->vars.push_back(current_vardef);
		}
		else if (context == Actor) {
			assert(current_actor != nullptr);
			current_actor->vars.push_back(current_vardef);
		}
		else if (context == Unit) {
			assert(current_unit != nullptr);
			current_unit->vars.push_back(current_vardef);
		}
		else {
			std::cout << "Error: Cannot assign VarDef." << std::endl;
			assert(0);
		}

		current_vardef = nullptr;
	}
}

void AST::AST_Builder::start_Action(void)
{
	context_stack.push_back(context);
	context = Action;
	current_action = new AST::Action();
}

void AST::AST_Builder::end_Action(void)
{
	assert(context == Action);
	assert(current_action != nullptr);
	assert(current_actor != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	current_actor->actions.push_back(current_action);
	current_action = nullptr;

	assert(context == Actor);
}

void AST::AST_Builder::start_InitAction(void)
{
	context_stack.push_back(context);
	context = InitAction;
	current_action = new AST::Action();
}

void AST::AST_Builder::end_InitAction(void)
{
	assert(context == InitAction);
	assert(current_action != nullptr);
	assert(current_actor != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	current_actor->init = current_action;
	current_action = nullptr;

	assert(context == Actor);
}

void AST::AST_Builder::start_Type(void)
{
	context_stack.push_back(context);
	context = Type;
	type_stack.push_back(current_type);
	current_type = new AST::Type();
}

void AST::AST_Builder::end_Type(void)
{
	assert(context == Type);
	assert(current_type != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	AST::Type* prev = type_stack.back();

	if (context == ActorParameter) {
		assert(current_actorparam != nullptr);
		current_actorparam->type = *current_type;
	}
	else if (context == Generator) {
		assert(current_expression_context.current_generator != nullptr);
		current_expression_context.current_generator->type = current_type;
	}
	else if (context == VarDefinition) {
		assert(current_vardef != nullptr);
		current_vardef->type = *current_type;
	}
	else if (context == Parameter) {
		assert(current_param != nullptr);
		current_param->type = *current_type;
	}
	else if (context == PortDecl) {
		assert(current_port != nullptr);
		current_port->type = *current_type;
	}
	else if (context == Function) {
		assert(current_function != nullptr);
		current_function->ret_type = *current_type;
	}
	else if (context == NativeFunction) {
		assert(current_nativefunc != nullptr);
		current_nativefunc->ret_type = *current_type;
	}
	else if (context == Type) {
		assert(current_type != nullptr);
		assert(prev != nullptr);
		prev->listtype = current_type;
	}
	else {
		std::cout << "Error: Cannot assign type." << std::endl;
		assert(0);
	}
	current_type = prev;
	type_stack.pop_back();
}

void AST::AST_Builder::start_Statement(void)
{
	context_stack.push_back(context);
	context = Statement;
}

void AST::AST_Builder::end_Statement(void)
{
	assert((context == IfStatement) || (context == ElseStatement) || (context == AssignmentStatement) ||
		(context == ForeachStatement) || (context == WhileStatement) || (context == BlockStatement) ||
		(context == CallStatement));

	ParserContext ending = context;
	context = context_stack.back();
	context_stack.pop_back();
	assert(current_statement != nullptr);
	AST::Statement* s = nullptr;
	if (!statement_stack.empty()) {
		s = statement_stack.back();
		statement_stack.pop_back();
	}

	if ((context == Action) || (context == InitAction)) {
		assert(current_action != nullptr);
		current_action->statements.push_back(current_statement);
	}
	else if (context == ForeachStatement) {
		AST::ForeachStatement* f = dynamic_cast<AST::ForeachStatement*>(s);
		assert(f != nullptr);
		f->statements.push_back(current_statement);
	}
	else if (context == WhileStatement) {
		AST::WhileStatement* w = dynamic_cast<AST::WhileStatement*>(s);
		assert(w != nullptr);
		w->statements.push_back(current_statement);
	}
	else if (context == IfStatement) {
		AST::IfStatement *i = dynamic_cast<AST::IfStatement*>(s);
		assert(i != nullptr);
		i->ifblock.push_back(current_statement);
	}
	else if (context == ElseStatement) {
		AST::ElseStatement* e = dynamic_cast<AST::ElseStatement*>(s);
		assert(e != nullptr);
		e->statements.push_back(current_statement);
	}
	else if (context == BlockStatement) {
		AST::BlockStatement* b = dynamic_cast<AST::BlockStatement*>(s);
		assert(b != nullptr);
		b->statements.push_back(current_statement);
	}
	else if (context == Procedure) {
		assert(current_procedure != nullptr);
		current_procedure->statements.push_back(current_statement);
	}
	else {
		std::cout << "Error: Statement unassigned." << std::endl;
		assert(0);
	}
	current_statement = s;
}

void AST::AST_Builder::start_IfStatement(void)
{
	context = IfStatement;
	if (current_statement != nullptr) {
		statement_stack.push_back(current_statement);
	}
	current_statement = new AST::IfStatement();
}

void AST::AST_Builder::start_ElseStatement(void)
{
	assert(context == IfStatement);
	context = ElseStatement;
	if (current_statement != nullptr) {
		statement_stack.push_back(current_statement);
	}
	current_statement = new AST::ElseStatement();
}

void AST::AST_Builder::end_IfStatement(void)
{
	if (context == ElseStatement) {
		AST::Statement *s = statement_stack.back();
		AST::IfStatement* i = dynamic_cast<AST::IfStatement*>(s);
		assert(i != nullptr);
		i->elseblock = dynamic_cast<AST::ElseStatement*>(current_statement);
		assert(i->elseblock != nullptr);
		current_statement = s;
		statement_stack.pop_back();
		context = IfStatement;
		end_Statement();
	}
	else if (context == IfStatement) {
		end_Statement();
	}
}

void AST::AST_Builder::start_CallStatement(void)
{
	context = CallStatement;
	if (current_statement != nullptr) {
		statement_stack.push_back(current_statement);
	}
	AST::CallStatement *c = new AST::CallStatement();
	c->name = statement_identifier;
	current_statement = c;
}

void AST::AST_Builder::end_CallStatement(void)
{
	assert(context == CallStatement);
	end_Statement();
}

void AST::AST_Builder::start_BlockStatement(void)
{
	context = BlockStatement;
	if (current_statement != nullptr) {
		statement_stack.push_back(current_statement);
	}
	current_statement = new AST::BlockStatement();
}

void AST::AST_Builder::end_BlockStatement(void)
{
	assert(context == BlockStatement);
	end_Statement();
}

void AST::AST_Builder::start_WhileStatement(void)
{
	context = WhileStatement;
	if (current_statement != nullptr) {
		statement_stack.push_back(current_statement);
	}
	current_statement = new AST::WhileStatement();
}

void AST::AST_Builder::end_WhileStatement(void)
{
	assert(context == WhileStatement);
	end_Statement();
}

void AST::AST_Builder::start_ForeachStatement(void)
{
	context = ForeachStatement;
	if (current_statement != nullptr) {
		statement_stack.push_back(current_statement);
	}
	current_statement = new AST::ForeachStatement();
}

void AST::AST_Builder::end_ForeachStatement(void)
{
	assert(context == ForeachStatement);
	end_Statement();
}

void AST::AST_Builder::start_AssignmentStatement(void)
{
	context = AssignmentStatement;
	if (current_statement != nullptr) {
		statement_stack.push_back(current_statement);
	}
	AST::AssignmentStatement *a = new AST::AssignmentStatement();
	a->identifier = statement_identifier;
	current_statement = a;
}

void AST::AST_Builder::end_AssignmentStatement(void)
{
	assert(context == AssignmentStatement);
	end_Statement();
}

void AST::AST_Builder::start_Index(void)
{
	context_stack.push_back(context);
	context = Index;
	current_expression_context.current_index = new AST::Index();
}

void AST::AST_Builder::end_Index(void)
{
	assert(context == Index);
	assert(current_expression_context.current_index != nullptr);
	context = context_stack.back();
	context_stack.pop_back();

	if (context == AssignmentStatement) {
		AST::AssignmentStatement* a = dynamic_cast<AST::AssignmentStatement*>(current_statement);
		assert(a != nullptr);
		a->indices.push_back(current_expression_context.current_index);
	}
	else {
		AST::Identifier* i = dynamic_cast<AST::Identifier*>(current_expression_context.current_operand);
		assert(i != nullptr);
		i->indices.push_back(current_expression_context.current_index);
	}
	current_expression_context.current_index = nullptr;
}

void AST::AST_Builder::start_CallExpression(void)
{
	context_stack.push_back(context);
	context = CallExpression;
	current_expression_context.current_call = new AST::Call();
}

void AST::AST_Builder::end_CallExpression(void)
{
	assert(context == CallExpression);
	assert(current_expression_context.current_call != nullptr);
	context = context_stack.back();
	context_stack.pop_back();

	AST::Identifier* i = dynamic_cast<AST::Identifier*>(current_expression_context.current_operand);
	assert(i != nullptr);
	i->call = current_expression_context.current_call;
	current_expression_context.current_call = nullptr;
}

void AST::AST_Builder::start_Operator(void)
{
	context_stack.push_back(context);
	context = Operator;
	auto o = new AST::Operator();

	assert(current_expression_context.current_expression != nullptr);

	if (current_expression_context.current_expression->child == nullptr) {
		current_expression_context.current_expression->child = o;
		o->left = current_expression_context.current_operand;
	}
	else if (current_expression_context.current_expression->child != nullptr) {
		AST::BaseExpression* b = current_expression_context.current_expression->child;
		AST::Operator* op = dynamic_cast<AST::Operator*>(b);
		if (op != nullptr) {
			if (op->right == nullptr) {
				assert(current_expression_context.current_operand != nullptr);
				op->right = current_expression_context.current_operand;
				current_expression_context.current_operand = nullptr;
			}
			else {
				//braket expr
				assert(dynamic_cast<AST::Expression*>(op->right) != nullptr);
			}
		}
		else {
			//braket expr
			assert(dynamic_cast<AST::Expression*>(b) != nullptr);
		}
		o->left = current_expression_context.current_expression->child;
		current_expression_context.current_expression->child = o;
	}
	else {
		o->left = current_expression_context.current_operand;
		current_expression_context.current_operator->right = o;
	}

	current_expression_context.current_operand = nullptr;
	current_expression_context.current_operator = o;
}

void AST::AST_Builder::end_Operator(void)
{
	assert(context == Operator);

	context = context_stack.back();
	context_stack.pop_back();
}

void AST::AST_Builder::start_Literal(void)
{
	context_stack.push_back(context);
	context = Literal;
	current_expression_context.current_operand = new AST::Literal();
}

void AST::AST_Builder::end_Literal(void)
{
	assert(context == Literal);

	context = context_stack.back();
	context_stack.pop_back();
}

void AST::AST_Builder::start_Expression(void)
{
#if 0
	if ((context == Expression) &&
		(current_expression_context.current_expression != nullptr) &&
		(current_expression_context.current_expression->child == nullptr))
	{
		/* Prevent creating nested expressions for brakets */
		return;
	}
#endif
	context_stack.push_back(context);
	context = Expression;
	expression_context_stack.push_back(current_expression_context);
	init_expression_context(&current_expression_context);
	current_expression_context.current_expression = new AST::Expression();
	if (next_expr_braket) {
		current_expression_context.current_expression->brakets = true;
		next_expr_braket = false;
	}
}

void AST::AST_Builder::end_Expression(void)
{
	assert(context == Expression);
	context = context_stack.back();
	context_stack.pop_back();

	if (current_expression_context.current_expression->child == nullptr) {
		assert(current_expression_context.current_operand != nullptr);
		current_expression_context.current_expression->child =
			current_expression_context.current_operand;
	}
	else if (current_expression_context.current_operator != nullptr) {
		assert(current_expression_context.current_operator->left != nullptr);
		current_expression_context.current_operator->right =
			current_expression_context.current_operand;
	}

	auto prev = expression_context_stack.back();

	if (context == Generator) {
		assert(prev.current_generator != nullptr);
		if (prev.current_generator->start == nullptr) {
			prev.current_generator->start = current_expression_context.current_expression;
		}
		else {
			prev.current_generator->end = current_expression_context.current_expression;
		}
	}
	else if (context == IfStatement) {
		AST::IfStatement* i = dynamic_cast<AST::IfStatement*>(current_statement);
		assert(i != nullptr);
		i->condition = current_expression_context.current_expression;
	}
	else if (context == TernaryExpression) {
		auto t = dynamic_cast<AST::TernaryOperator*>(prev.current_operand);
		assert(t != nullptr);
		if (t->cond == nullptr) {
			t->cond = current_expression_context.current_expression;
		}
		else if (t->ifblock == nullptr) {
			t->ifblock = current_expression_context.current_expression;
		}
		else if (t->elseblock == nullptr) {
			t->elseblock = current_expression_context.current_expression;
		}
		else {
			std::cout << "Error: Cannot assign Expression to TernaryOperator." << std::endl;
			assert(0);
		}
	}
	else if (context == OutputExpression) {
		assert(current_outputexpr != nullptr);
		current_outputexpr->expressions.push_back(current_expression_context.current_expression);
	}
	else if (context == Guard) {
		assert(current_action != nullptr);
		current_action->guards.push_back(current_expression_context.current_expression);
	}
	else if (context == Repeat) {
		auto c = context_stack.back();
		if (c == InputPattern) {
			assert(current_inputpattern != nullptr);
			current_inputpattern->repeat = current_expression_context.current_expression;
		}
		else if (c == OutputExpression) {
			assert(current_outputexpr != nullptr);
			current_outputexpr->repeat = current_expression_context.current_expression;
		}
		else {
			std::cout << "Error: Unassigned Repeat!" << std::endl;
			assert(0);
		}
	}
	else if (context == VarDefinition) {
		assert(current_vardef != nullptr);
		current_vardef->assign = current_expression_context.current_expression;
	}
	else if (context == Array) {
		assert(current_vardef != nullptr);
		current_vardef->arrays.push_back(current_expression_context.current_expression);
	}
	else if (context == WhileStatement) {
		auto w = dynamic_cast<AST::WhileStatement*>(current_statement);
		assert(w != nullptr);
		w->condition = current_expression_context.current_expression;
	}
	else if (context == CallExpression) {
		assert(prev.current_call != nullptr);
		prev.current_call->parameters.push_back(current_expression_context.current_expression);
	}
	else if (context == CallStatement) {
		auto c = dynamic_cast<AST::CallStatement*>(current_statement);
		assert(c != nullptr);
		c->parameters.push_back(current_expression_context.current_expression);
	}
	else if (context == Index) {
		assert(prev.current_index != nullptr);
		prev.current_index->index = current_expression_context.current_expression;
	}
	else if (context == AssignmentStatement) {
		auto a = dynamic_cast<AST::AssignmentStatement*>(current_statement);
		assert(a != nullptr);
		a->asgnvalue = current_expression_context.current_expression;
	}
	else if (context == ListComprehension) {
		assert(prev.current_listcomprehension != nullptr);
		prev.current_listcomprehension->expressions.push_back(current_expression_context.current_expression);
	}
	else if (context == Type) {
		assert(current_type != nullptr);
		current_type->size = current_expression_context.current_expression;
	}
	else if (context == Function) {
		assert(current_function != nullptr);
		current_function->expression = current_expression_context.current_expression;
	}
	else if (context == ActorParameter) {
		assert(current_actorparam != nullptr);
		current_actorparam->asign = current_expression_context.current_expression;
	}
	else if (context == Expression) {
		assert(prev.current_expression != nullptr);
		if (prev.current_expression->child == nullptr) {
			prev.current_expression->child = current_expression_context.current_expression;
		}
		else {
			auto op = dynamic_cast<AST::Operator*>(prev.current_expression->child);
			assert(op != nullptr);
			op->right = current_expression_context.current_expression;
			prev.current_operator = nullptr;
		}
	}
	else {
		std::cout << "Error: Cannot assign Expression." << std::endl;
		assert(0);
	}

	current_expression_context = prev;
	expression_context_stack.pop_back();
}

void AST::AST_Builder::start_TernaryExpression(void)
{
	context_stack.push_back(context);
	context = TernaryExpression;
	current_expression_context.current_operand = new AST::TernaryOperator();
}

void AST::AST_Builder::end_TernaryExpression(void)
{
	if (context == TernaryExpression) {
		context = context_stack.back();
		context_stack.pop_back();
	}
}

void AST::AST_Builder::start_UnaryOp(void)
{
	context_stack.push_back(context);
	context = UnaryOp;
	current_expression_context.current_unaryop = new AST::UnaryOperator();
}

void AST::AST_Builder::end_UnaryOp(void)
{
	assert(context == UnaryOp);
	assert(current_expression_context.current_unaryop != nullptr);
	context = context_stack.back();
	context_stack.pop_back();

	if (current_expression_context.current_operand != nullptr) {
		AST::Identifier* i = dynamic_cast<AST::Identifier*>(current_expression_context.current_operand);
		assert(i != nullptr);
		assert(i->unary_right == nullptr);
		i->unary_right = current_expression_context.current_unaryop;
		current_expression_context.current_unaryop = nullptr;
	}
	else {
		/* no assignment possible, yet. Identifier will be created in the next step! */
	}
}

void AST::AST_Builder::start_ListComprehension(void)
{
	context_stack.push_back(context);
	context = ListComprehension;
	current_expression_context.current_listcomprehension = new AST::ListComprehension();
}

void AST::AST_Builder::end_ListComprehension(void)
{
	assert(context == ListComprehension);
	assert(current_expression_context.current_listcomprehension != nullptr);
	assert(current_expression_context.current_expression->child == nullptr);
	context = context_stack.back();
	context_stack.pop_back();

	current_expression_context.current_expression->child =
		current_expression_context.current_listcomprehension;
	current_expression_context.current_listcomprehension = nullptr;
}

void AST::AST_Builder::start_Generator(void)
{
	context_stack.push_back(context);
	context = Generator;
	current_expression_context.current_generator = new AST::Generator();
}

void AST::AST_Builder::end_Generator(void)
{
	assert(context == Generator);
	assert(current_expression_context.current_generator != nullptr);
	context = context_stack.back();
	context_stack.pop_back();

	if (context == ForeachStatement) {
		AST::ForeachStatement* f = dynamic_cast<AST::ForeachStatement*>(current_statement);
		assert(f != nullptr);
		f->generators.push_back(current_expression_context.current_generator);
	}
	else if (context == ListComprehension) {
		assert(current_expression_context.current_listcomprehension != nullptr);
		current_expression_context.current_listcomprehension->generators.push_back(
			current_expression_context.current_generator);
	}
	else {
		std::cout << "Error: Cannot assign Generator." << std::endl;
		assert(0);
	}
	current_expression_context.current_generator = nullptr;
}

void AST::AST_Builder::start_Procedure(void)
{
	context_stack.push_back(context);
	context = Procedure;
	current_procedure = new AST::Procedure();
}

void AST::AST_Builder::end_Procedure(void)
{
	if (context == Procedure) {
		assert(current_procedure != nullptr);
		context = context_stack.back();
		context_stack.pop_back();

		if (context == Actor) {
			assert(current_actor != nullptr);
			current_actor->procedures.push_back(current_procedure);
		}
		else if (context == Unit) {
			assert(current_unit != nullptr);
			current_unit->procedures.push_back(current_procedure);
		}
		else {
			std::cout << "Error: Procedure without assignment." << std::endl;
			assert(0);
		}
		current_procedure = nullptr;
	}
	else {
		assert(context == NativeProcedure);
	}
}

void AST::AST_Builder::start_Function(void)
{
	context_stack.push_back(context);
	context = Function;
	current_function = new AST::Function();


}

void AST::AST_Builder::end_Function(void)
{
	/* Function is called for native and for function end! */
	if (context == Function) {
		assert(current_function != nullptr);
		context = context_stack.back();
		context_stack.pop_back();

		if (context == Actor) {
			assert(current_actor != nullptr);
			current_actor->functions.push_back(current_function);
		}
		else if (context == Unit) {
			assert(current_unit != nullptr);
			current_unit->functions.push_back(current_function);
		}
		else {
			std::cout << "Error: Function without assignment." << std::endl;
			assert(0);
		}
		current_function = nullptr;
	}
	else {
		assert(context == NativeFunction);
	}
}

void AST::AST_Builder::start_NativeFunction(void)
{
	context_stack.push_back(context);
	context = NativeFunction;
	current_nativefunc = new AST::NativeFunction();
}

void AST::AST_Builder::end_NativeFunction(void)
{
	/* Function is called for native and for function end! */
	if (context == NativeFunction) {
		assert(current_nativefunc != nullptr);
		context = context_stack.back();
		context_stack.pop_back();

		if (context == Actor) {
			assert(current_actor != nullptr);
			current_actor->nativefunctions.push_back(current_nativefunc);
		}
		else if (context == Unit) {
			assert(current_unit != nullptr);
			current_unit->nativefunctions.push_back(current_nativefunc);
		}
		else {
			std::cout << "Error: NativeFunction without assignment." << std::endl;
			assert(0);
		}
		current_nativefunc = nullptr;
	}
}

void AST::AST_Builder::start_NativeProcedure(void)
{
	context_stack.push_back(context);
	context = NativeProcedure;
	current_nativeproc = new AST::NativeProcedure();
}

void AST::AST_Builder::end_NativeProcedure(void)
{
	/* Function is called for native and for function end! */
	if (context == NativeProcedure) {
		assert(current_nativeproc != nullptr);
		context = context_stack.back();
		context_stack.pop_back();

		if (context == Actor) {
			assert(current_actor != nullptr);
			current_actor->nativeprocedures.push_back(current_nativeproc);
		}
		else if (context == Unit) {
			assert(current_unit != nullptr);
			current_unit->nativeprocedures.push_back(current_nativeproc);
		}
		else {
			std::cout << "Error: NativeProcedure without assignment." << std::endl;
			assert(0);
		}
		current_nativeproc = nullptr;
	}
}

void AST::AST_Builder::start_InputPattern(void)
{
	context_stack.push_back(context);
	context = InputPattern;
	current_inputpattern = new AST::Input_Pattern();
}

void AST::AST_Builder::end_InputPattern(void)
{
	assert(context == InputPattern);
	assert(current_inputpattern != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	assert(current_action != nullptr);
	current_action->input_patterns.push_back(current_inputpattern);
	current_inputpattern = nullptr;
}

void AST::AST_Builder::start_OutputExpression(void)
{
	context_stack.push_back(context);
	context = OutputExpression;
	current_outputexpr = new AST::Output_Expression();
}

void AST::AST_Builder::end_OutputExpression(void)
{
	assert(context == OutputExpression);
	assert(current_outputexpr != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	assert(current_action != nullptr);
	current_action->output_expressions.push_back(current_outputexpr);
	current_outputexpr = nullptr;
}

void AST::AST_Builder::start_Repeat(void)
{
	context_stack.push_back(context);
	context = Repeat;
}

void AST::AST_Builder::end_Repeat(void)
{
	assert(context == Repeat);

	context = context_stack.back();
	context_stack.pop_back();
}

void AST::AST_Builder::start_Guard(void)
{
	context_stack.push_back(context);
	context = Guard;
}

void AST::AST_Builder::end_Guard(void)
{
	assert(context == Guard);

	context = context_stack.back();
	context_stack.pop_back();

	assert(context == Action);
}

void AST::AST_Builder::start_scheduleFSM(void)
{
	context_stack.push_back(context);
	context = ScheduleFSM;
	current_schedulefsm = new AST::Schedule_FSM();
}

void AST::AST_Builder::end_scheduleFSM(void)
{
	assert(context == ScheduleFSM);
	assert(current_schedulefsm != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	assert(current_actor != nullptr);
	current_actor->fsm = current_schedulefsm;
	current_schedulefsm = nullptr;

	assert(context == Actor);
}

void AST::AST_Builder::start_StateTransition(void)
{
	context_stack.push_back(context);
	context = StateTransition;
	current_fsmstate = new AST::Schedule_FSM_State();
}

void AST::AST_Builder::end_StateTransition(void)
{
	if (context != StateTransition) {
		assert(context == ScheduleFSM);
		return;
	}
	assert(current_fsmstate != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	assert(current_schedulefsm != nullptr);
	current_schedulefsm->transitions.push_back(current_fsmstate);
	current_fsmstate = nullptr;

	assert(context == ScheduleFSM);
}

void AST::AST_Builder::start_PriorityInequality(void)
{
	context_stack.push_back(context);
	context = PriorityInequality;
	current_prio = new AST::Action_Priority();
}

void AST::AST_Builder::end_PriorityInequality(void)
{
	if (context != PriorityInequality) {
		assert(context == Actor);
		return;
	}

	assert(current_prio != nullptr);
	context = context_stack.back();
	context_stack.pop_back();
	assert(current_actor != nullptr);
	current_actor->prios.push_back(current_prio);
	current_prio = nullptr;

	assert(context == Actor);
}

void AST::AST_Builder::start_IDList(void)
{
	context_stack.push_back(context);
	context = IDList;
	idlist.name.clear();
}

void AST::AST_Builder::end_IDList(void)
{
	context = context_stack.back();
	context_stack.pop_back();

	if (context == Import) {
		assert(current_import != nullptr);
		AST::ID x;
		auto split = idlist.name.find_last_of(".");
		x.line = idlist.line;
		x.character = idlist.character + static_cast<unsigned>(split) + 1;
		x.name = "*";
		idlist.name = idlist.name.substr(0, split);

		current_import->path = idlist;
		current_import->symbol = x;
	}
	else if (context == Action) {
		assert(current_action != nullptr);
		current_action->name = idlist;
	}
	else if (context == StateTransition) {
		assert(current_fsmstate != nullptr);
		current_fsmstate->actions.push_back(idlist);
	}
	else if (context == PriorityInequality) {
		assert(current_prio != nullptr);
		current_prio->prio_rel.push_back(idlist);
	}
	else if (context == File) {
		/* presumably package, ignore */
	}
	else {
		std::cout << "Error: Cannot assign IDList." << std::endl;
		assert(0);
	}
	idlist.name.clear();
}

void AST::AST_Builder::start_Array(void)
{
	context_stack.push_back(context);
	context = Array;
}

void AST::AST_Builder::end_Array(void)
{
	assert(context == Array);
	context = context_stack.back();
	context_stack.pop_back();
}