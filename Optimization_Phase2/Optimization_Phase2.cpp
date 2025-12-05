#include "Optimization_Phase2.hpp"
#include "Config/config.h"
#include "Actor_Merge/Merge_Optimization.hpp"

Optimization::Optimization_Data_Phase2* Optimization::optimize_phase2(IR::Dataflow_Network* dpn) {

	Config* c = c->getInstance();

	if (c->get_optimize_core_merge()) {
		Merge_Optimization::core_merge(dpn);
	}

	/* Intentionally left rather empty, for future use. */

	return new Optimization::Optimization_Data_Phase2{};
}