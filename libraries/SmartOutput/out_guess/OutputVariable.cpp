#include "OutputVariable.h"
#include <cctype> //for std::tolower

const int out_guess::OutputVariable::days_in_month_[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

out_guess::OutputVariable::OutputVariable():
	id_(-1),
	time_domain_(TimeDomain::Invalid), 
	reduction_type_(TimeReductionType::Invalid),
	daily_(false),
	monthly_(false),
	yearly_(false)
{
}

out_guess::OutputVariable::OutputVariable(std::string name, std::string unit, TimeDomain time_domain, TimeReductionType reduction_type):
	id_(-1),
	daily_(false),
	monthly_(false),
	yearly_(false)
{

	//Make all names lowercase to avoid conflicts with netcdf
	name_ = ToLower(name);
	unit_ = unit;
	time_domain_ = time_domain;
	reduction_type_ = reduction_type;
}

out_guess::OutputVariable::~OutputVariable()
{
}

void out_guess::OutputVariable::SetId(int id)
{
	id_ = id;
}

int out_guess::OutputVariable::GetId() const
{
	return id_;
}

void out_guess::OutputVariable::AccStand()
{
	if (daily_)
		for (int i = 0; i < vals_stand_day_2D->GetSizeTotal(); ++i)
			vals_stand_day_2D->Acc(i, vals_patch_day_2D->Get(i));

	if (monthly_)
		for (int i = 0; i < vals_stand_month_2D->GetSizeTotal(); ++i)
			vals_stand_month_2D->Acc(i, vals_patch_month_2D->Get(i));
		
	if (yearly_)
		for (int i = 0; i < vals_stand_year_2D->GetSizeTotal(); ++i)
			vals_stand_year_2D->Acc(i, vals_patch_year_2D->Get(i));
}

void out_guess::OutputVariable::ClearPatch()
{
	if (daily_)
		ClearArrayV(vals_patch_day_2D);
	
	if (monthly_)
		ClearArrayV(vals_patch_month_2D);

	if (yearly_)
		ClearArrayV(vals_patch_year_2D);
}

void out_guess::OutputVariable::ClearStand()
{
	if (daily_)
		ClearArrayV(vals_stand_day_2D);

	if (monthly_)
		ClearArrayV(vals_stand_month_2D);

	if (yearly_)
		ClearArrayV(vals_stand_year_2D);
}

void out_guess::OutputVariable::RescalePatch(double scale)
{
	if (daily_)
		RescaleArrayV(vals_patch_day_2D, scale);


	if (monthly_)
		RescaleArrayV(vals_patch_month_2D, scale);


	if (yearly_)
		RescaleArrayV(vals_patch_year_2D, scale);
}

void out_guess::OutputVariable::RescaleStand(double scale)
{
	if (daily_)
		RescaleArrayV(vals_stand_day_2D, scale);

	if (monthly_)
		RescaleArrayV(vals_stand_month_2D, scale);

	if (yearly_)
		RescaleArrayV(vals_stand_year_2D, scale);
}

void out_guess::OutputVariable::TimeRescalePatch()
{
	switch (reduction_type_)
	{
	case TimeReductionType::Mean:
		MeanTimeCalculation();
		break;

	case TimeReductionType::Total:
		//Nothing to be done here as this is the standard procedure
		break;

	default:
		break;
	}
}

std::string out_guess::OutputVariable::GetName() const
{
	return name_;
}

std::string out_guess::OutputVariable::GetUnit() const
{
	return unit_;
}

out_guess::TimeReductionType out_guess::OutputVariable::GetReductionType() const
{
	return reduction_type_;
}

out_guess::TimeDomain out_guess::OutputVariable::GetTimeDomain() const
{
	return time_domain_;
}

ArrayV2D<double>* out_guess::OutputVariable::GetStandDay2D() const
{
	return vals_stand_day_2D.get();
}

ArrayV2D<double>* out_guess::OutputVariable::GetStandMonth2D() const
{
	return vals_stand_month_2D.get();
}

ArrayV2D<double>* out_guess::OutputVariable::GetStandYear2D() const
{
	return vals_stand_year_2D.get();
}

//ArrayV1D<double>* out_guess::OutputVariable::GetStandDay1D() const
//{
//	return  vals_stand_day_1D.get();
//}
//
//ArrayV1D<double>* out_guess::OutputVariable::GetStandMonth1D() const
//{
//	return  vals_stand_month_1D.get();
//}
//
//ArrayV1D<double>* out_guess::OutputVariable::GetStandYear1D() const
//{
//	return  vals_stand_year_1D.get();
//}

//ArrayV<double>& out_guess::OutputVariable::GetStandDataDay() const
//{
//	return *vals_stand_day;
//}
//
//ArrayV<double>& out_guess::OutputVariable::GetStandDataMonth() const
//{
//	return *vals_stand_month;
//}
//
//ArrayV<double>& out_guess::OutputVariable::GetStandDataYear() const
//{
//	return *vals_stand_year;
//}

bool out_guess::OutputVariable::IsDaily() const
{
	return daily_;
}

bool out_guess::OutputVariable::IsMonthly() const
{
	return monthly_;
}

bool out_guess::OutputVariable::isYearly() const
{
	return yearly_;
}


void out_guess::OutputVariable::RescaleArrayV(std::shared_ptr<ArrayV<double>> arr, const double scale)
{
	if (arr != nullptr)
		for (double& value : *arr)
			value *= scale;
}

void out_guess::OutputVariable::ClearArrayV(std::shared_ptr<ArrayV<double>> arr)
{
	if (arr != nullptr)
		for (double& value : *arr)
			value = 0.0;
}

std::string out_guess::OutputVariable::ToLower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return std::tolower(c); } // correct
	);
	return s;
}