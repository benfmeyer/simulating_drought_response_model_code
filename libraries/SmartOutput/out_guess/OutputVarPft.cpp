#include "OutputVarPft.h"
#include "ArrayV2D.h"


out_guess::OutputVarPft::OutputVarPft():OutputVariable(),pft_detail_level_(PftDetailLevel::Invalid)
{
}

out_guess::OutputVarPft::OutputVarPft(std::string name, std::string unit, 
	PftDetailLevel pft_detail_level,
	TimeDomain time_domain,
	TimeReductionType time_red_type):
	OutputVariable(name, unit, time_domain, time_red_type),
pft_detail_level_(pft_detail_level)
{
	switch (time_domain_)
	{

	case TimeDomain::Daily:
	{
		switch (pft_detail_level)
		{
		case PftDetailLevel::Pfts_Total:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, npft + 1);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, npft + 1);
			break;

		case PftDetailLevel::Total:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			break;
		default: break;
		}
		daily_ = true;

	}
	break;

	case TimeDomain::Monthly:
	{
		switch (pft_detail_level)
		{
		case PftDetailLevel::Pfts_Total:
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			break;

		case PftDetailLevel::Total:
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			break;
		default: break;
		}
		monthly_ = true;
	}
	break;

	case TimeDomain::Yearly:
	{
		switch (pft_detail_level)
		{
		case PftDetailLevel::Pfts_Total:
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, npft + 1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, npft + 1);
			break;

		case PftDetailLevel::Total:
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1,1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1,1);
			break;
		default: break;
		}
		yearly_ = true;
	}
	break;

	case TimeDomain::Daily_Monthly:
	{
		switch (pft_detail_level)
		{
		case PftDetailLevel::Pfts_Total:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, npft + 1);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, npft + 1);
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			break;

		case PftDetailLevel::Total:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			break;
		default: break;
		}
		daily_ = true;
		monthly_ = true;
	}
	break;

	case TimeDomain::Monthly_Yearly:
	{
		switch (pft_detail_level)
		{
		case PftDetailLevel::Pfts_Total:
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, npft + 1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, npft + 1);
			break;

		case PftDetailLevel::Total:
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			break;
		default: break;
		}
		monthly_ = true;
		yearly_ = true;
	}
	break;

	case TimeDomain::Daily_Monthly_Yearly:
	{
		switch (pft_detail_level)
		{
		case PftDetailLevel::Pfts_Total:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, npft + 1);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, npft + 1);
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npft + 1);
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, npft + 1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, npft + 1);
			break;

		case PftDetailLevel::Total:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			break;
		default: break;
		}
		daily_ = true;
		monthly_ = true;
		yearly_ = true;
	}
	break;

	default:
		break;
	}
}

out_guess::OutputVarPft::~OutputVarPft()
{
}

void out_guess::OutputVarPft::AccPftValue(const double value, const int pft_id, Date date)
{
	switch (pft_detail_level_)
	{
	case PftDetailLevel::Total:
		if (daily_)
			vals_patch_day_2D->Acc(date.day, 0 , value);

		if (monthly_)
		{
			if (reduction_type_ == TimeReductionType::Last)
			{
				if (date.islastday)
				{
					vals_patch_month_2D->Acc(date.month, 0, value);
				}
			}
			else
			{
				vals_patch_month_2D->Acc(date.month, 0, value);
			}
		}

		if (yearly_)
		{
			if (reduction_type_ == TimeReductionType::Last)
			{
				if (date.islastday && date.islastmonth)
				{
					vals_patch_year_2D->Acc(0, 0, value);
				}
			}
			else
			{
				vals_patch_year_2D->Acc(0, 0, value);
			}

		}
		break;

	case PftDetailLevel::Pfts_Total:
		if (daily_)
		{
			vals_patch_day_2D->Acc(date.day, pft_id, value);
			vals_patch_day_2D->Acc(date.day, npft, value);	 //This is the column for the total values
		}

		if (monthly_)
		{
			if (reduction_type_ == TimeReductionType::Last)
			{
				if (date.islastday)
				{
					vals_patch_month_2D->Acc(date.month, pft_id, value);
					vals_patch_month_2D->Acc(date.month, npft, value); //This is the column for the total values
				}
			}
			else
			{
				vals_patch_month_2D->Acc(date.month, pft_id, value);
				vals_patch_month_2D->Acc(date.month, npft, value); //This is the column for the total values
			}

		}

		if (yearly_)
		{
			if (reduction_type_ == TimeReductionType::Last)
			{
				if (date.islastday && date.islastmonth)
				{
					vals_patch_year_2D->Acc(0, pft_id, value);
					vals_patch_year_2D->Acc(0, npft, value);			//This is the column for the total values
				}
			}

			else
			{
				vals_patch_year_2D->Acc(0, pft_id, value);
				vals_patch_year_2D->Acc(0, npft, value);			//This is the column for the total values
			}

		}
		break;
	default:
		break;
	}
}

out_guess::PftDetailLevel out_guess::OutputVarPft::GetPftDetailLevel() const
{
	return pft_detail_level_;
}

void out_guess::OutputVarPft::MeanTimeCalculation()
{
	//if (daily_) nothing to be done here, as lowest output resolution is daily!	
	if (monthly_)
	{
		//This could be improved with rows or colums.		
		int cols = vals_patch_month_2D->GetSizeY();
		for (int i = 0; i < cols; ++i)
		{
			for (int t = 0; t < 12; ++t)
			{
				const double value = vals_patch_month_2D->Get(t, i);
				vals_patch_month_2D->Set(t, i, value / days_in_month_[t]);
			}
		}
	}

	if (yearly_)
	{
		for (double& value : *vals_patch_year_2D)
			value *= 1.0 / 365.0;
	}
}

void out_guess::OutputVarPft::RescalePatch(double scale)
{
	OutputVariable::RescalePatch(scale);
}

void out_guess::OutputVarPft::RescaleStand(double scale)
{
	OutputVariable::RescaleStand(scale);
}
