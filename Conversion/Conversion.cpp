#include "Conversion.hpp"
#include "common/include/Exceptions.hpp"

static int evaluate_expr(AST::BaseExpression* e, std::map<std::string, std::string>& symbol_map)
{
	if (dynamic_cast<AST::Operator*>(e) != nullptr) {
		AST::Operator* o = dynamic_cast<AST::Operator*>(e);
		int left = evaluate_expr(o->left, symbol_map);
		int right = evaluate_expr(o->right, symbol_map);
		if (o->ops == "+") {
			return left + right;
		}
		else if (o->ops == "-") {
			return left - right;
		}
		else if (o->ops == "*") {
			return left * right;
		}
		else if (o->ops == "/") {
			return left / right;
		}
		else if (o->ops == "%") {
			return left % right;
		}
		else {
			throw Converter_Exception{ "Unknown Operation in const expr: " + o->ops + "." };
		}
	}
	else if (dynamic_cast<AST::Literal*>(e) != nullptr) {
		int tmp = std::stoi(dynamic_cast<AST::Literal*>(e)->literal);
		if (dynamic_cast<AST::Literal*>(e)->negation) {
			return -tmp;
		}
		return tmp;
	}
	else if (dynamic_cast<AST::Identifier*>(e) != nullptr) {
		if (!symbol_map.contains(dynamic_cast<AST::Identifier*>(e)->identifier)) {
			throw Converter_Exception{ "Identifier not in constant list." };
		}
		return std::stoi(symbol_map[dynamic_cast<AST::Identifier*>(e)->identifier]);
	}
	else if (dynamic_cast<AST::Expression*>(e) != nullptr) {
		return evaluate_expr(dynamic_cast<AST::Expression*>(e)->child, symbol_map);
	}
	else {
		throw Converter_Exception{ "Unknown AST element in const expr." };
	}
}

int Conversion_Helper::evaluate_constant_expression(
	AST::Expression* expression,
	std::map<std::string, std::string>& symbol_map)
{
	return evaluate_expr(expression, symbol_map);
}

void Conversion_Helper::read_constants(
	AST::VarDefinition *var,
	std::map<std::string, std::string>& symbol_map)
{
	if (!var->constassign || (var->assign == nullptr)) {
		return;
	}
	if (dynamic_cast<AST::ListComprehension*>(var->assign->child) != nullptr) {
		return;
	}
	int value = evaluate_constant_expression(var->assign, symbol_map);
	symbol_map[var->name.name] = std::to_string(value);
}