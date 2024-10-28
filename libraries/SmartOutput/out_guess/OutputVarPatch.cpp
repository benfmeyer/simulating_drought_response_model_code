#include "OutputVarPatch.h"
#include "ArrayV2D.h"

out_guess::OutputVarPatch::OutputVarPatch(): OutputVariable(), detail_level_(PatchDetailLevel::Invalid)
{
}

out_guess::OutputVarPatch::OutputVarPatch(std::string name, std::string unit,
	PatchDetailLevel detail_level, TimeDomain time_domain, TimeReductionType reduction_type):
OutputVariable(name,unit, time_domain, reduction_type)
{
	detail_level_ = detail_level;
	time_reduction_type_ = reduction_type;

	switch (time_domain_)
	{

	case TimeDomain::Daily:
	{
		switch (detail_level)
		{
		case PatchDetailLevel::Each_Patch:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, npatch);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, npatch);
			break;
		case PatchDetailLevel::Mean_over_all:
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
		switch (detail_level)
		{
		case PatchDetailLevel::Each_Patch:
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);
			break;
		case PatchDetailLevel::Mean_over_all:
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
		switch (detail_level)
		{
		case PatchDetailLevel::Each_Patch:
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, npatch);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, npatch);
			break;
		case PatchDetailLevel::Mean_over_all:
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			break;
		default: break;
		}
		yearly_ = true;
	}
	break;

	case TimeDomain::Daily_Monthly:
	{
		switch (detail_level)
		{
		case PatchDetailLevel::Each_Patch:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, npatch);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, npatch);
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);

			break;
		case PatchDetailLevel::Mean_over_all:
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
		switch (detail_level)
		{
		case PatchDetailLevel::Each_Patch:
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, npatch);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, npatch);
			break;
		case PatchDetailLevel::Mean_over_all:
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
		switch (detail_level)
		{
		case PatchDetailLevel::Each_Patch:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, npatch);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, npatch);
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, npatch);
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, npatch);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, npatch);
			break;

		case PatchDetailLevel::Mean_over_all:
			vals_patch_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			vals_stand_day_2D = std::make_shared<ArrayV2D<double>>(365, 1);
			vals_patch_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_stand_month_2D = std::make_shared<ArrayV2D<double>>(12, 1);
			vals_patch_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			vals_stand_year_2D = std::make_shared<ArrayV2D<double>>(1, 1);
			break;

		default:
			break;
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

void out_guess::OutputVarPatch::SetPatchValue(const double value, const int patch_id, Date date) const
{
	switch (time_reduction_type_)
	{
	case TimeReductionType::Last:
	{
		switch (detail_level_)
		{
		case PatchDetailLevel::Each_Patch:
			if (daily_)
				vals_patch_day_2D->Set(date.day, patch_id, value);

			if (monthly_)
				if (date.islastday)
					vals_patch_month_2D->Set(date.month, patch_id, value);			

			if (yearly_)
				if (date.day == 364)
					vals_patch_year_2D->Set(0, patch_id, value);
			break;

		case PatchDetailLevel::Mean_over_all:
			if (daily_)
				vals_patch_day_2D->Acc(date.day, 0, value);

			if (monthly_)
				if (date.islastday)
					vals_patch_month_2D->Acc(date.month, 0, value);

			if (yearly_)
				if (date.day == 364)
					vals_patch_year_2D->Acc(0, 0, value);
			break;
		default:
			break;
		}
	}
	break;



	default:
	{
		switch (detail_level_)
		{
		case PatchDetailLevel::Each_Patch:
			if (daily_)
				vals_patch_day_2D->Set(date.day, patch_id, value);

			if (monthly_)
				vals_patch_month_2D->Acc(date.month, patch_id, value);

			if (yearly_)
				vals_patch_year_2D->Acc(0, patch_id, value);
			break;

		case PatchDetailLevel::Mean_over_all:
			if (daily_)
				vals_patch_day_2D->Acc(date.day, 0, value);

			if (monthly_)
				vals_patch_month_2D->Acc(date.month, 0, value);

			if (yearly_)
				vals_patch_year_2D->Acc(0, 0, value);
			break;
		default:
			break;
		}
	}
	}




}

void out_guess::OutputVarPatch::RescalePatch(double scale)
{
	switch (detail_level_)
	{
	case PatchDetailLevel::Each_Patch: 
		break;
	case PatchDetailLevel::Mean_over_all:
		OutputVariable::RescalePatch(scale);
		break;
	case PatchDetailLevel::Invalid: 
		break;
	default:
		break;
	}
}

void out_guess::OutputVarPatch::RescaleStand(const double scale)
{
	switch (detail_level_)
	{
	case PatchDetailLevel::Each_Patch:
		break;
	case PatchDetailLevel::Mean_over_all:
		OutputVariable::RescaleStand(scale);
		break;
	case PatchDetailLevel::Invalid:
		break;
	default:
		break;
	}
}

out_guess::PatchDetailLevel out_guess::OutputVarPatch::GetPatchDetailLevel() const
{
	return detail_level_;
}

out_guess::OutputVarPatch::~OutputVarPatch()
{
}

void out_guess::OutputVarPatch::MeanTimeCalculation()
{
	switch (detail_level_)
	{
	case PatchDetailLevel::Each_Patch:
	{
		if (monthly_)
		{
			//std::shared_ptr<ArrayV2D<double>> arr2D = std::dynamic_pointer_cast<ArrayV2D<double>>(vals_patch_month);

			for (int i = 0; i < npatch; ++i)
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
	break;
	case PatchDetailLevel::Mean_over_all:
	{
		if (monthly_)
		{
			for (int t = 0; t < 12; ++t)
			{
				double value = vals_patch_month_2D->Get(t);
				vals_patch_month_2D->Set(t,0 , value / days_in_month_[t]);
			}
		}

		if (yearly_)
		{
			for (double& value : *vals_patch_year_2D)
				value *= 1.0 / 365.0;
		}

	}
	break;
	default:
	break;
	}
}
