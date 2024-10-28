#pragma once
#include "OutputVariable.h"
namespace out_guess
{
	class OutputVarPft : public OutputVariable
	{
	public:
		OutputVarPft();
		OutputVarPft(std::string name,
			std::string unit, 
			PftDetailLevel pft_detail_level,
			TimeDomain time_domain,
			TimeReductionType time_red_type);

		~OutputVarPft() override;
		void RescalePatch(double scale) override;
		void RescaleStand(double scale) override;
		void AccPftValue(const double value, const int pft_id, Date date);

		PftDetailLevel GetPftDetailLevel() const;
	protected:
		void MeanTimeCalculation() override;
		PftDetailLevel pft_detail_level_;

	};
}

