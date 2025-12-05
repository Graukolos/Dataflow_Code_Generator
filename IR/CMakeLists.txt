add_subdirectory(AST)

target_sources(Dataflow_Code_Generator PRIVATE
	IR_Transform.cpp)