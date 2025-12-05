#pragma once

#include "AST.hpp"

namespace AST {
	void print_ast(AST::AST_Root* ast);

#include <string>
	void print_statement(
		AST::Statement* s,
		std::string prefix);
}