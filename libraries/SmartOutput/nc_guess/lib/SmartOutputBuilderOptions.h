///////////////////////////////////////////////////////////////////////////////////////
/// \file SmartOutputBuilderOptions.h
/// \brief Options that are used/changed by SmartOutput
/// \author Phillip Papastefanou
/// $Date: 2017-02-11 23:17:02 +0100 (Sun, 11 Feb 2017) $
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "internal_out/OutTypes.h"
#include "Types.h"
#include <Types.h>
#include "commandlinearguments.h"

struct SmartOutputBuilderOptions
{
	/**
	 * \brief Default SmartOutputBuilderOptions
	 */
	SmartOutputBuilderOptions();
	~SmartOutputBuilderOptions();

	/// File Format of the netcdf files
	nc_internal::DataFormat data_format;
	/// wheter to supress all daily output even if variables are defined with daily output
	bool supress_daily_output;
	/// wheter to supress all monthly output even if variables are defined with monthly output
	bool supress_monthly_output;
	/// wheter to supress all annual output even if variables are defined with annual output
	bool supress_annualy_output;
	/// the time range to chose for the output
	OutputTimeRange time_range;
	/// the beginning year to put out (only valid if time_range == Custom)
	int year_begin;
	/// the end year to put out (inclusive!)(only valid if time_range == Custom)
	int year_end;
	/// should there be verbose output
	bool verbose;
	/// Print function used, if verbose output is set to true
	void Print(const std::string& message);

	InputModuleType input_module_type;
};

