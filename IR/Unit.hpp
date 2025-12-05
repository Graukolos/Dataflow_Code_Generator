#pragma once
#include <string>
#include "AST/AST.hpp"
#include <vector>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "AST_Analysis/AST_Analysis.hpp"

namespace IR {

	class Dataflow_Network;

	class Unit {
	private:

		std::string code;
		bool initialized{ false };
		std::vector<Unit*> sub_units;

		AST::AST_Root* ast;

		void convert_imports(IR::Dataflow_Network* dpn);

	public:
		
		Unit(std::string c) : code{ c } {
			ast = nullptr;
		}

		void initialize(IR::Dataflow_Network *dpn) {
			if (initialized) {
				return;
			}
			initialized = true;

			Lexer::Lexer l{ code };
			Parser::Parser_Class p{ &l };
			ast = p.parse();

			convert_imports(dpn);
		}

		AST::AST_Root* get_ast(void) {
			return ast;
		}

		std::vector<AST::AST_Root*> get_imported_units(void)
		{
			std::vector<AST::AST_Root*> ret;

			for (auto x : sub_units) {
				ret.push_back(x->get_ast());
			}

			return ret;
		}

		std::vector<Unit*>& get_sub_units(void)
		{
			return sub_units;
		}
	};
}