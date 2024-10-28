#include "SmartOutputBuilderOptions.h"
#include <iostream>


SmartOutputBuilderOptions::SmartOutputBuilderOptions():
	supress_daily_output(false),
	supress_monthly_output(false),
	supress_annualy_output(false),
	time_range(OutputTimeRange::Scenario),
	year_begin(-1),
	year_end(-1),
	verbose(false),
	data_format(nc_internal::DataFormat::NC4_HDF5)
{

	// With parallel usage daily output is suppressed by default
	#ifdef HAVE_PNETCDF
	supress_daily_output = true;
	#endif
}


SmartOutputBuilderOptions::~SmartOutputBuilderOptions()
{
}

void SmartOutputBuilderOptions::Print(const std::string& message)
{
	if (verbose)
		std::cout << message;
}
