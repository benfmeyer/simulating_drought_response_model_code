#include "PatchCollector.h"


out_guess::PatchCollector::PatchCollector(): var_id_running_index_(0)
{
}

out_guess::PatchCollector::~PatchCollector()
{
}


void out_guess::PatchCollector::AddVar(OutputVarPatch& var)
{
	var.SetId(var_id_running_index_);
	out_var_refs_.push_back(std::make_unique<OutputVarPatch>(var));
	var_id_running_index_++;
}

void out_guess::PatchCollector::SetValuePerPatch(OutputVarPatch& var, double value, int patch_id, Date date)
{
	if (var.GetId() == -1)
	{
		const std::string error = "Variable " + var.GetName() + " has not been added to a collector, did you forget this ?";
		throw OutGuessException(error);
	}

	out_var_refs_[var.GetId()]->SetPatchValue(value, patch_id, date);
}

void out_guess::PatchCollector::AccValPerStand()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->AccStand();
	}
}

void out_guess::PatchCollector::ClearPatch()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->ClearPatch();
	}
}

void out_guess::PatchCollector::ClearStand()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->ClearStand();
	}
}

void out_guess::PatchCollector::TimeRescalePatch()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->TimeRescalePatch();
	}
}


void out_guess::PatchCollector::RescalePatch(double scale)
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->RescalePatch(scale);
	}
}

