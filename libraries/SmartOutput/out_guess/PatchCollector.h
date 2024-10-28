#pragma once
#include "OutputVarPatch.h"


namespace out_guess
{

class PatchCollector
{
public:
	PatchCollector();
	~PatchCollector();

	void AddVar(OutputVarPatch& var);
	void SetValuePerPatch(OutputVarPatch& var, double value, int patch_id, Date date);
	void AccValPerStand();
	void ClearPatch();
	void ClearStand();
	void TimeRescalePatch();

	std::vector<std::shared_ptr<OutputVarPatch>> out_var_refs_;

	void RescalePatch(double scale);

private:
	int var_id_running_index_;
};

}

