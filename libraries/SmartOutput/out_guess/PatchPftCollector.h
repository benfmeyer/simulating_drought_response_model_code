#pragma once
#include <vector>
#include "OutputVarPft.h"

namespace out_guess
{
	class PatchPftCollector
	{
	public:
		PatchPftCollector();
		~PatchPftCollector();

		void AddVar(OutputVarPft& var);
		void AccValPerPatch(OutputVarPft& var, double value, int pft_id, Date date);
		void AccValPerStand();
		void ClearPatch();
		void ClearStand();

		void TimeRescalePatch();

		std::vector<std::shared_ptr<OutputVarPft>> out_var_refs_;

		void RescalePatch(double scale);

	private:
		int var_id_running_index_;

	};

}

