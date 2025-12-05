#include "AST_Helper.hpp"
#include <iostream>

static bool check_call(AST::Call* c, unsigned check_val);
static bool check_generator(AST::Generator* g, unsigned check_val);

static bool check_baseexpression(
	AST::BaseExpression* b,
	unsigned check_val)
{
	if (b->check_val == check_val) {
		return false;
	}
	b->check_val = check_val;

	bool ret = true;

	if (dynamic_cast<AST::Literal*>(b) != nullptr) {
		auto e = dynamic_cast<AST::Literal*>(b);
	}
	else if (dynamic_cast<AST::Identifier*>(b) != nullptr) {
		auto e = dynamic_cast<AST::Identifier*>(b);
		for (auto i : e->indices) {
			ret &= check_baseexpression(i->index, check_val);
		}
		if (e->call != nullptr) {
			ret &= check_call(e->call, check_val);
		}
	}
	else if (dynamic_cast<AST::TernaryOperator*>(b) != nullptr) {
		auto e = dynamic_cast<AST::TernaryOperator*>(b);
		ret &= check_baseexpression(e->cond, check_val);
		ret &= check_baseexpression(e->ifblock, check_val);
		ret &= check_baseexpression(e->elseblock, check_val);
	}
	else if (dynamic_cast<AST::ListComprehension*>(b) != nullptr) {
		auto e = dynamic_cast<AST::ListComprehension*>(b);
		for (auto x : e->expressions) {
			ret &= check_baseexpression(x, check_val);
		}
		for (auto g : e->generators) {
			ret &= check_generator(g, check_val);
		}
	}
	else if (dynamic_cast<AST::Operator*>(b) != nullptr) {
		auto e = dynamic_cast<AST::Operator*>(b);
		ret &= check_baseexpression(e->left, check_val);
		ret &= check_baseexpression(e->right, check_val);
	}
	else if (dynamic_cast<AST::Expression*>(b) != nullptr) {
		auto e = dynamic_cast<AST::Expression*>(b);
		ret = check_baseexpression(e->child, check_val);
	}
	else if (dynamic_cast<AST::PortPreview*>(b) != nullptr) {
		auto e = dynamic_cast<AST::PortPreview*>(b);
		if (e->index != nullptr) {
			ret = check_baseexpression(e->index->index, check_val);
		}
	}
	else if (dynamic_cast<AST::PortSize*>(b) != nullptr) {
		auto e = dynamic_cast<AST::PortSize*>(b);
	}
	else if (dynamic_cast<AST::PortFree*>(b) != nullptr) {
		auto e = dynamic_cast<AST::PortFree*>(b);
		
	}
	else if (dynamic_cast<AST::FSM_Enumeration_Element*>(b) != nullptr) {
		
	}
	else {
		std::cout << "Unknown";
	}

	b->check_val = check_val - 1;

	return ret;
}

static bool check_call(
	AST::Call* c,
	unsigned check_val)
{
	if (c->check_val == check_val) {
		return false;
	}
	c->check_val = check_val;

	bool ret = true;

	for (auto p : c->parameters) {
		ret &= check_baseexpression(p, check_val);
	}

	c->check_val = check_val - 1;

	return ret;
}

static bool check_vardef(
	AST::VarDefinition* var,
	unsigned check_val)
{
	if (var->check_val == check_val) {
		return false;
	}
	var->check_val = check_val;

	bool ret = true;

	for (auto a : var->arrays) {
		ret &= check_baseexpression(a, check_val);
	}
	if (var->assign != nullptr) {
		ret &= check_baseexpression(var->assign, check_val);
	}

	var->check_val = check_val - 1;

	return ret;
}

static bool check_generator(
	AST::Generator* g,
	unsigned check_val)
{
	if (g->check_val == check_val) {
		return false;
	}
	g->check_val = check_val;

	bool ret = true;

	ret &= check_baseexpression(g->start, check_val);
	ret &= check_baseexpression(g->end, check_val);

	g->check_val = check_val - 1;

	return ret;
}

static bool check_statement(
	AST::Statement* s,
	unsigned check_val)
{
	if (s->check_val == check_val) {
		return false;
	}
	s->check_val = check_val;

	bool ret = true;

	if (dynamic_cast<AST::IfStatement*>(s) != nullptr) {
		auto i = dynamic_cast<AST::IfStatement*>(s);
		ret &= check_baseexpression(i->condition, check_val);
		for (auto s : i->ifblock) {
			ret &= check_statement(s, check_val);
		}
		if (i->elseblock != nullptr) {
			for (auto s : i->elseblock->statements) {
				ret &= check_statement(s, check_val);
			}
		}
	}
	else if (dynamic_cast<AST::CallStatement*>(s) != nullptr) {
		auto c = dynamic_cast<AST::CallStatement*>(s);
		for (auto p : c->parameters) {
			ret &= check_baseexpression(p, check_val);
		}
	}
	else if (dynamic_cast<AST::BlockStatement*>(s) != nullptr) {
		auto b = dynamic_cast<AST::BlockStatement*>(s);
		for (auto v : b->vars) {
			ret &= check_vardef(v, check_val);
		}
		for (auto s : b->statements) {
			ret &= check_statement(s, check_val);
		}
	}
	else if (dynamic_cast<AST::WhileStatement*>(s) != nullptr) {
		auto w = dynamic_cast<AST::WhileStatement*>(s);
		ret &= check_baseexpression(w->condition, check_val);
		for (auto v : w->vars) {
			ret &= check_vardef(v, check_val);
		}
		for (auto s : w->statements) {
			ret &= check_statement(s, check_val);
		}
	}
	else if (dynamic_cast<AST::ForeachStatement*>(s) != nullptr) {
		auto f = dynamic_cast<AST::ForeachStatement*>(s);
		for (auto g : f->generators) {
			ret &= check_generator(g, check_val);
		}
		for (auto v : f->vars) {
			ret &= check_vardef(v, check_val);
		}
		for (auto s : f->statements) {
			ret &= check_statement(s, check_val);
		}
	}
	else if (dynamic_cast<AST::InputChannelReadStatement*>(s) != nullptr) {
		auto i = dynamic_cast<AST::InputChannelReadStatement*>(s);
		if (i->index != nullptr) {
			ret = check_baseexpression(i->index->index, check_val);
		}
	}
	else if (dynamic_cast<AST::OutputChannelWriteStatement*>(s) != nullptr) {
		auto o = dynamic_cast<AST::OutputChannelWriteStatement*>(s);
		ret = check_baseexpression(o->expr, check_val);
	}
	else if (dynamic_cast<AST::AssignmentStatement*>(s) != nullptr) {
		auto a = dynamic_cast<AST::AssignmentStatement*>(s);
		for (auto x : a->indices) {
			ret &= check_baseexpression(x->index, check_val);
		}
		ret &= check_baseexpression(a->asgnvalue, check_val);
	}
	else if (dynamic_cast<AST::ReturnStatement*>(s) != nullptr) {
		/* ret is already true, nothing to do */
	}
	else {
		std::cout << "Unknown Statement\n";
	}

	s->check_val = check_val - 1;

	return ret;
}

bool AST::check_cycle(AST::Statement* s)
{
	unsigned check_val = (s->check_val + 1) % 17;
	return check_statement(s, check_val);
}

bool AST::check_cycle_list(std::vector<AST::Statement*> s)
{
	for (auto x : s) {
		bool r = AST::check_cycle(x);
		if (!r) {
			return false;
		}
	}
	return true;
}