//////////////////////////////////////////////////////////////////////////////////////
/// \file NetCDF_Common.h
/// \brief Base class for NetCDF input modules
/// adapted from cfinput.h/cpp
/// 
/// \author Phillip Papastefanou
/// $Date: 2017-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include "guessnc.h"
#include "cruinput.h"
#include "ICoordParser.h"
#include "CoordI.h"
#include <fstream>
#include "CfNdepInputConst.h"
#include <limits>


class NetCDF_Common
{
public:
	NetCDF_Common();
	~NetCDF_Common();

protected:
	void init_driver_files();
	void check_driver_files();
	void init_landcover_and_management();


	void check_temp_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var);
	void check_vpd_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var);
	void check_relhum_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var);
	void check_prec_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var);
	void check_windspeed_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var);
	void check_insol_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var);
	void check_wetdays_variable(const GuessNC::CF::GridcellOrderedVariable* cf_var);

	insoltype cf_standard_name_to_insoltype(const std::string& standard_name);

	bool later_day(const Date& date,
		const GuessNC::CF::DateTime& date_time);



	void check_compatible_timeseries(const GuessNC::CF::GridcellOrderedVariable* var1,
		const GuessNC::CF::GridcellOrderedVariable* var2);
	void check_compatible_timeseries(const std::vector<GuessNC::CF::GridcellOrderedVariable*> variables);
	void check_same_spatial_domains(const std::vector<GuessNC::CF::GridcellOrderedVariable*> variables);
	GuessNC::CF::DateTime last_day_to_simulate(const GuessNC::CF::GridcellOrderedVariable* cf_var);


	/// Loads data from NetCDF files for current grid cell
	/** Returns the coordinates for the current grid cell, for
	*  the closest CRU grid cell and the soilcode for the cell.
	*  \returns whether it was possible to load data and find nearby CRU cell */
	bool load_data_from_files(double& lon, double& lat);

	/// Gets the first few years of data from cf_var and puts it into spinup_data
	void load_spinup_data(const GuessNC::CF::GridcellOrderedVariable* cf_var,
		GenericSpinupData& spinup_data);


	/** Returns either 12 or 365/366 values (depending on LPJ-GUESS year length, not
*  data set year length). Gets the values from spinup and/or historic period. */
	void get_yearly_data(std::vector<double>& data,
		const GenericSpinupData& spinup,
		GuessNC::CF::GridcellOrderedVariable* cf_historic,
		int& historic_timestep);

	/** Returns either 12 or 365/366 values (depending on LPJ-GUESS year length, not
	*  data set year length). Gets the values from spinup and/or historic period. */
	void get_yearly_data_no_spinup(std::vector<double>& data, double const_spinup_value,
		GuessNC::CF::GridcellOrderedVariable* cf_historic,
		int& historic_timestep);


	/// Fills one array of daily values with forcing data for the current year
	void populate_daily_array(double* daily,
		const GenericSpinupData& spinup,
		GuessNC::CF::GridcellOrderedVariable* cf_historic,
		int& historic_timestep,
		double minimum = -std::numeric_limits<double>::max(),
		double maximum = std::numeric_limits<double>::max());


	/// Same as populate_daily_array, but for precipitation which is special
	/** Uses number of wet days if available and handles extensive/intensive conversion */
	void populate_daily_prec_array(long& seed);


	/// Fills dtemp, dprec, etc. with forcing data for the current year
	void populate_daily_arrays(Gridcell& gridcell);


	/// \returns all (used) variables
	std::vector<GuessNC::CF::GridcellOrderedVariable*> all_variables() const;




	static const int NYEAR_SPINUP_DATA = 9;

	/// Land cover input module
	LandcoverInput landcover_input;
	/// Management input module
	ManagementInput management_input;

	SoilInput soilinput;

	/// The grid cells to simulate
	std::vector<CoordI> gridlist;

	/// The current grid cell to simulate
	CoordI current_gridcell;
	bool is_finished_{};

	///Added by PP
	bool is_reduced_;

	/// Is this the first gridcell to simulate
	bool is_first_gridcell;

	size_t n_gridcells{};
	size_t first_simulation_year{};
	size_t last_simulation_year{};

	/// Runtime needed for loading input files
	bool is_first_year;
	double elapsed_input_ms{};


    /// Equals to one if relative humidity is in the range from [0-1]
    /// and to 100 if relative humiditiy is in percent [0-100%]
    double rel_hum_multiplier{};

	/// Yearly CO2 data read from file
    /**
    * This object is indexed with calendar years, so to get co2 value for
    * year 1990, use co2[1990]. See documentation for GlobalCO2File for
    * more information.
    */
	GlobalCO2File co2;

	// The variables

	GuessNC::CF::GridcellOrderedVariable* cf_temp;

	GuessNC::CF::GridcellOrderedVariable* cf_prec;

	GuessNC::CF::GridcellOrderedVariable* cf_insol;

	GuessNC::CF::GridcellOrderedVariable* cf_vpd;
	GuessNC::CF::GridcellOrderedVariable* cf_wind;
	GuessNC::CF::GridcellOrderedVariable* cf_relhum;

	GuessNC::CF::GridcellOrderedVariable* cf_pres;

	GuessNC::CF::GridcellOrderedVariable* cf_specifichum;


	GuessNC::CF::GridcellOrderedVariable* cf_wetdays;

	GuessNC::CF::GridcellOrderedVariable* cf_min_temp;

	GuessNC::CF::GridcellOrderedVariable* cf_max_temp;

	std::unique_ptr<CfNdepInputConst> ncdf_ndep;

	// Spinup data for each variable

	GenericSpinupData spinup_temp;

	GenericSpinupData spinup_prec;

	GenericSpinupData spinup_insol;

	GenericSpinupData spinup_vpd;
	GenericSpinupData spinup_hurs;
	GenericSpinupData spinup_windspeed;

	GenericSpinupData spinup_wetdays;

	GenericSpinupData spinup_min_temp;

	GenericSpinupData spinup_max_temp;

	GenericSpinupData spinup_pres;

	GenericSpinupData spinup_specifichum;

	/// Temperature for current gridcell and current year (deg C)
	double dtemp[Date::MAX_YEAR_LENGTH]{};

	/// Precipitation for current gridcell and current year (mm/day)
	double dprec[Date::MAX_YEAR_LENGTH]{};

	/// Insolation for current gridcell and current year (\see instype)
	double dinsol[Date::MAX_YEAR_LENGTH]{};

	/// Daily N deposition for one year
	double dNH4dep[Date::MAX_YEAR_LENGTH]{}, dNO3dep[Date::MAX_YEAR_LENGTH]{};

	/// Minimum temperature for current gridcell and current year (deg C)
	double dmin_temp[Date::MAX_YEAR_LENGTH]{};

	/// Maximum temperature for current gridcell and current year (deg C)
	double dmax_temp[Date::MAX_YEAR_LENGTH]{};

	/// Daily temperature range for current gridcell and current year (deg C)
	double ddtr[Date::MAX_YEAR_LENGTH]{};

	/// Vapor pressure deficit for current gridcell and current year
	double dvpd[Date::MAX_YEAR_LENGTH]{};
	/// Windspeed for current gridcell and year (m/s)
	double dwindspeed[Date::MAX_YEAR_LENGTH]{};
	/// Relative humidity for current gridcell and year (%)
	double dhurs[Date::MAX_YEAR_LENGTH]{};
	/// daily pressure (Pa)
	double dpres[Date::MAX_YEAR_LENGTH]{};

	/// daily specifichum (kg/kg)
	double dspecifichum[Date::MAX_YEAR_LENGTH]{};


	/// Whether the forcing data for precipitation is an extensive quantity
	/** If given as an amount (kg m-2) per timestep it is extensive, if it's
	*  given as a mean rate (kg m-2 s-1) it is an intensive quantity */
	bool extensive_precipitation{};

	// Current timestep in CF files

	int historic_timestep_temp{};

	int historic_timestep_prec{};

	int historic_timestep_insol{};

	int historic_timestep_vpd{};
	int historic_timestep_hurs{};
	int historic_timestep_windspeed{};

	int historic_timestep_pres{};

	int historic_timestep_specifichum{};


	int historic_timestep_wetdays{};

	int historic_timestep_min_temp{};

	int historic_timestep_max_temp{};

	/// Nitrogen deposition forcing for current gridcell
	Lamarque::NDepData ndep;

	/// Nitrogen deposition time series to use (historic,rcp26,...)
	std::string ndep_timeseries;

	// Timers for keeping track of progress through the simulation
	Timer tprogress, tmute;
	static const int MUTESEC = 20; // minimum number of sec to wait between progress messages

	bool verbose{};
	bool load_balancing{};

	/// Check if input celsius has to transformed to kelvin
	bool reduce_celsius_to_kelvin{};
	/// <summary>
	/// Tentens equation for vpd based on temperature and hurs
	/// </summary>
	/// <param name="temperature"> Surface temperature in �C</param>
	/// <param name="hurs"> Relative humidity in %</param>
	/// <returns>VPD in kPA (negative units)</returns>
	double get_vpd_by_temp_and_hurs(double temperature, double hurs);


	int parseLine(char* line);
	int get_virtual_memory();
	int get_physical_memory();
	



	private:
		bool first_day_of_year(GuessNC::CF::DateTime dt);
		bool first_month_of_year(GuessNC::CF::DateTime dt);
		bool is_daily(const GuessNC::CF::GridcellOrderedVariable* cf_var);
		bool earlier_day(const Date& date, int calendar_year,
			const GuessNC::CF::DateTime& date_time);


		double max_insolation(insoltype instype);


};

