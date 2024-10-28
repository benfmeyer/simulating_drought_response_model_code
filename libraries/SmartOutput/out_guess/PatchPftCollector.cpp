#include "PatchPftCollector.h"
#include <iostream>

out_guess::PatchPftCollector::PatchPftCollector(): var_id_running_index_(0)
{

}


out_guess::PatchPftCollector::~PatchPftCollector()
{

}

void out_guess::PatchPftCollector::AddVar(OutputVarPft& var)
{
	var.SetId(var_id_running_index_);
	out_var_refs_.push_back(std::make_unique<OutputVarPft>(var));
	var_id_running_index_++;
}

void out_guess::PatchPftCollector::AccValPerPatch(OutputVarPft& var, double value, int pft_id, Date date)
{
	if (var.GetId() == -1)
	{
		std::string error = "Variable " + var.GetName() + " has not beed added to a collector, did you forget this ?";
		throw OutGuessException(error);
	}

	out_var_refs_[var.GetId()]->AccPftValue(value, pft_id, date);
}

void out_guess::PatchPftCollector::AccValPerStand()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->AccStand();
	}
}

void out_guess::PatchPftCollector::ClearPatch()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->ClearPatch();
	}
}

void out_guess::PatchPftCollector::ClearStand()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->ClearStand();
	}
}

void out_guess::PatchPftCollector::TimeRescalePatch()
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->TimeRescalePatch();
	}
}

void out_guess::PatchPftCollector::RescalePatch(double scale)
{
	for (auto& out_var : out_var_refs_)
	{
		out_var->RescalePatch(scale);
	}
}