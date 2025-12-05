#pragma once

#include <string>
#include <map>
#include "IR/AST/AST.hpp"

namespace Conversion_Helper {

	int evaluate_constant_expression(
		AST::Expression *expression,
		std::map<std::string, std::string>& symbol_map);

	void read_constants(
		AST::VarDefinition *var,
		std::map<std::string, std::string>& symbol_map);
}