#pragma once
#include "OutputVariable.h"

namespace out_guess
{
	class OutputVarPatch :
		public OutputVariable
	{
	public:
		OutputVarPatch();
		OutputVarPatch(std::string name, 
			std::string unit, 
			PatchDetailLevel detail_level, 
			TimeDomain time_domain,
			TimeReductionType reduction_type);

		~OutputVarPatch() override;

		void SetPatchValue(const double value, const int patch_id, Date date) const;
		void RescalePatch(double scale) override;
		void RescaleStand(double scale) override;

		PatchDetailLevel GetPatchDetailLevel() const;

	protected:
		void MeanTimeCalculation() override;

	private:
		PatchDetailLevel detail_level_;
		TimeReductionType time_reduction_type_;
	};

}
