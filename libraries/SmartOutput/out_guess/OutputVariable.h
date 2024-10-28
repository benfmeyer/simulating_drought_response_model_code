#pragma once
#include "ArrayV.h"
#include "Types.h"
#include "guess.h"
#include "ArrayV2D.h"
#include "ArrayV1D.h"
#include <memory>

namespace out_guess
{
	class OutputVariable
	{
	public:
		OutputVariable();
		OutputVariable(std::string name, std::string unit, TimeDomain time_domain, TimeReductionType reduction_type);
		//pure virtual destructor to avoid initialising
		virtual ~OutputVariable();

		void SetId(int id);
		int GetId() const;
		
		void AccStand();
		void ClearPatch();
		void ClearStand();

		virtual void RescalePatch(double scale);
		virtual void RescaleStand(double scale);

		void TimeRescalePatch();

		std::string GetName() const;
		std::string GetUnit() const;
		TimeReductionType GetReductionType() const;
		TimeDomain GetTimeDomain() const;

		ArrayV2D<double>* GetStandYear2D() const;
		ArrayV2D<double>* GetStandDay2D() const;
		ArrayV2D<double>* GetStandMonth2D() const;
		
		bool IsDaily() const;
		bool IsMonthly() const;
		bool isYearly() const;
		
	protected:
		int id_;
		std::string name_;
		std::string unit_;
		static const int days_in_month_[];

		TimeDomain time_domain_;
		TimeReductionType reduction_type_;

		bool daily_;
		bool monthly_;
		bool yearly_;

		std::shared_ptr<ArrayV2D<double> > vals_patch_day_2D;
		std::shared_ptr<ArrayV2D<double> > vals_patch_month_2D;
		std::shared_ptr<ArrayV2D<double> > vals_patch_year_2D;

		std::shared_ptr<ArrayV2D<double> > vals_stand_day_2D;
		std::shared_ptr<ArrayV2D<double> > vals_stand_month_2D;
		std::shared_ptr<ArrayV2D<double> > vals_stand_year_2D;

		virtual void MeanTimeCalculation() = 0;

	private: 
		void RescaleArrayV(std::shared_ptr<ArrayV<double>> arr, const double scale);
		void ClearArrayV(std::shared_ptr<ArrayV<double>> arr);
		std::string ToLower(std::string s);
	};
}

