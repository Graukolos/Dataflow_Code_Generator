#target_include_directories(Dataflow_Code_Generator PUBLIC ${CMAKE_CURRENT_LIST_DIR})


target_sources(Dataflow_Code_Generator PRIVATE
	AST_Builder.cpp
	AST_Helper.cpp
	AST_Printer.cpp)