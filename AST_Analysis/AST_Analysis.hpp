#pragma once

#include "IR/AST/AST.hpp"

namespace AST_Analysis {

	void semantic(std::vector<AST::Unit*> units, AST::AST_Root* ast);

};