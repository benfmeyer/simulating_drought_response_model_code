#include "NcInputParallelAutoSingle.h"
#include "NcInputParallelAuto.h"
#include <chrono>

#ifdef HAVE_NETCDF
#ifdef HAVE_MPI

#include "guess.h"
#include "driver.h"
#include "guessstring.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "parallel.h"

#ifdef WIN32
#include "windows.h"
#include "psapi.h"
#endif

#ifdef linux
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <stdexcept>
#endif

using namespace GuessNC::CF;

NcInputParallelAutoSingle::NcInputParallelAutoSingle(InputModuleType type):InputModule(type)
{

}


NcInputParallelAutoSingle::~NcInputParallelAutoSingle()
{
	global_log.flush();
	global_log.close();
}

void NcInputParallelAutoSingle::init(const CommandLineArguments& args) {

	this->verbose = args.get_verbose();
	this->load_balancing = args.get_load_balancing();

	if (Parallel::get().is_root())
	{
		this->global_log.open(global_log_filename);
		this->global_log << "Global LPJ-GUESS Simulation\n";
		this->global_log.flush();
	}
	
	init_driver_files();

	check_driver_files();

	if (verbose)
		global_log << nc_guess_internal::print_message("Initiating handler") << std::endl;

	// Get gridlist
	mpi_gridlist_handler = std::make_unique<nc_guess_internal::MPI_Gridlist_Data_Send>(is_reduced_, *cf_temp, global_log, verbose, this);
	
	//This function should be called only by root
	mpi_gridlist_handler->SendInitialCoords();

	//This function should be called by all other processes other than root
	mpi_gridlist_handler->ReceiveNextCoord();

	current_gridcell = mpi_gridlist_handler->GetNextCoord();
	is_finished_ = mpi_gridlist_handler->GetIsFinished();

	n_gridcells = mpi_gridlist_handler->GetNumberOfGridcells();



	first_simulation_year = cf_temp->get_date_time(0).get_year();
	last_simulation_year = cf_temp->get_date_time(cf_temp->get_timesteps() - 1).get_year();

	mpi_gridlist_handler->first_year = first_simulation_year;
	mpi_gridlist_handler->last_year = last_simulation_year;

	init_landcover_and_management();

	date.set_first_calendar_year(cf_temp->get_date_time(0).get_year() - nyear_spinup);

    soilinput.init(param["file_soildata"].str);

	// Set timers
	tprogress.init();
	tmute.init();

	
	tprogress.settimer();
	tmute.settimer(MUTESEC);

}

bool NcInputParallelAutoSingle::getgridcell(Gridcell& gridcell)
{

	double lon, lat;
	double cru_lon, cru_lat;
	int soilcode;

	if (is_first_gridcell)
	{
		while (!is_finished_ && !load_data_from_files(
			current_gridcell.lon,
			current_gridcell.lat))
		{
			mpi_gridlist_handler->SendDoneToRoot(false);
			mpi_gridlist_handler->ReceiveNextCoord();
			is_finished_ = mpi_gridlist_handler->GetIsFinished();
			current_gridcell = mpi_gridlist_handler->GetNextCoord();
		}

		is_first_gridcell = false;
	}

	else
	{
		//mpi_gridlist_handler->SendDoneToRoot();
		mpi_gridlist_handler->ReceiveNextCoord();
		is_finished_ = mpi_gridlist_handler->GetIsFinished();
		current_gridcell = mpi_gridlist_handler->GetNextCoord();


		while (!is_finished_ && !load_data_from_files(
			current_gridcell.lon,
			current_gridcell.lat))
		{
			mpi_gridlist_handler->SendDoneToRoot(false);
			mpi_gridlist_handler->ReceiveNextCoord();
			is_finished_ = mpi_gridlist_handler->GetIsFinished();
			current_gridcell = mpi_gridlist_handler->GetNextCoord();
		}

	}
	
	// Simulation finished
	if (is_finished_)
	{
		//mpi_gridlist_handler->SendDoneToRoot();
		return false;
	}

	if (run_landcover) {
		bool LUerror = false;
		LUerror = landcover_input.loadlandcover(cru_lon, cru_lat);
		if (!LUerror)
			LUerror = management_input.loadmanagement(cru_lon, cru_lat);
		if (LUerror) {
			dprintf("\nError: could not find stand at (%g,%g) in landcover/management data file(s)\n", cru_lon, cru_lat);
			return false;
		}
	}

    lon = current_gridcell.lon;
    lat = current_gridcell.lat;

	gridcell.set_coordinates(lon, lat);
	//PP
	gridcell.set_index(current_gridcell.index);
	gridcell.set_id(current_gridcell.landid);
	gridcell.set_total_counts(n_gridcells, mpi_gridlist_handler->GetNumberOfGridcells());
	gridcell.set_time_begin_end(first_simulation_year, last_simulation_year);

	// Load spinup data for all variables
	load_spinup_data(cf_temp, spinup_temp);
	load_spinup_data(cf_prec, spinup_prec);
	load_spinup_data(cf_insol, spinup_insol);

	// --> PPX-Hydraulics
	if (cf_vpd) {
		load_spinup_data(cf_vpd, spinup_vpd);
	}
	else {
		if (cf_relhum)
			load_spinup_data(cf_relhum, spinup_hurs);
	}

	if (cf_wind) {
		load_spinup_data(cf_wind, spinup_windspeed);
	}
	// <-- PPX-Hydraulics

	if (cf_wetdays) {
		load_spinup_data(cf_wetdays, spinup_wetdays);
	}

	if (cf_min_temp) {
		load_spinup_data(cf_min_temp, spinup_min_temp);
	}

	if (cf_max_temp) {
		load_spinup_data(cf_max_temp, spinup_max_temp);
	}

    if (cf_pres) {
        load_spinup_data(cf_pres, spinup_pres);
    }

    if (cf_specifichum) {
        load_spinup_data(cf_specifichum, spinup_specifichum);
    }

	spinup_temp.detrend_data();

	soilinput.get_soil(lon, lat, gridcell);

	gridcell.climate.instype = cf_standard_name_to_insoltype(cf_insol->get_standard_name());

	historic_timestep_temp = -1;
	historic_timestep_prec = -1;
	historic_timestep_insol = -1;
	historic_timestep_wetdays = -1;
	historic_timestep_min_temp = -1;
	historic_timestep_max_temp = -1;
	// --> PPX-Hydraulics
	historic_timestep_vpd = -1;
	historic_timestep_hurs = -1;
	historic_timestep_windspeed = 1;
    historic_timestep_pres = -1;
    historic_timestep_specifichum = -1;
	// <-- PPX-Hydraulics


    cru_lon = floor(lon * 2.0) / 2.0 + 0.25;
    cru_lat = floor(lat * 2.0) / 2.0 + 0.25;

	dprintf("\nCommencing simulation for gridcell at (%g,%g)\n", lon, lat);
	if (current_gridcell.descrip != "") {
		dprintf("Description: %s\n", current_gridcell.descrip.c_str());
	}
	dprintf("Using soil code and Nitrogen deposition for (%3.1f,%3.1f)\n", cru_lon, cru_lat);

	return true;

}

bool NcInputParallelAutoSingle::getclimate(Gridcell& gridcell)
{
	if (is_first_year)
	{
		is_first_year = false;
		elapsed_input_ms = 0.0;
	}

	Climate& climate = gridcell.climate;

	GuessNC::CF::DateTime last_date = last_day_to_simulate(cf_temp);

	if (later_day(date, last_date)) {
		// Gridcell is over, reset first year onset.
		is_first_year = true;
		return false;
	}

	climate.co2 = co2[date.get_calendar_year()];

	if (date.day == 0) {
		auto start_input = std::chrono::high_resolution_clock::now();

		populate_daily_arrays(gridcell);

		auto end_input = std::chrono::high_resolution_clock::now();
		elapsed_input_ms += std::chrono::duration_cast<std::chrono::milliseconds>(end_input - start_input).count();
	}

	climate.temp = dtemp[date.day];
	climate.prec = dprec[date.day];
	climate.insol = dinsol[date.day];


	// --> PPX-Hydraulics
    if (cf_vpd)
        climate.vpd = dvpd[date.day] > 0.0 ? 0.0 : dvpd[date.day];
    else if (cf_relhum || cf_specifichum && cf_pres ){
        climate.vpd = get_vpd_by_temp_and_hurs(dtemp[date.day],dhurs[date.day]);
        climate.vpd = climate.vpd > 0.0 ? 0.0: climate.vpd;
    }
    else{
        if(hydraulic_system != hydraulicsystemtype::STANDARD){
            fail("No (1)VPD or (2)relative humidity, or (3)specific humidity and pressure but hydraulics is configured!");
        }
    }

    if (cf_wind)
        climate.windspeed = dwindspeed[date.day] < 0.0 ? 0.0 : dwindspeed[date.day];
    else{
        if(hydraulic_system != hydraulicsystemtype::STANDARD){
            fail("No wind driver configured, but hydraulics is configured!");
        }
    }
	// <-- PPX-Hydraulics

    climate.u10 = dwindspeed[date.day];
    climate.tmax = dmax_temp[date.day];
    climate.tmin = dmin_temp[date.day];
    climate.dtr = ddtr[date.day];


    // Nitrogen deposition
    gridcell.dNH4dep = dNH4dep[date.day];
    gridcell.dNO3dep = dNO3dep[date.day];



    // bvoc
	if (ifbvoc) {
		if (cf_min_temp && cf_max_temp) {
			climate.dtr = dmax_temp[date.day] - dmin_temp[date.day];
		}
		else {
			fail("When BVOC is switched on, valid paths for minimum and maximum temperature must be given.");
		}
	}

	// First day of year only ...

	if (date.day == 0) {

		// Progress report to user and update timer

		if (tmute.getprogress() >= 1.0) {

			int first_historic_year = cf_temp->get_date_time(0).get_year();
			int last_historic_year = cf_temp->get_date_time(cf_temp->get_timesteps() - 1).get_year();
			int historic_years = last_historic_year - first_historic_year + 1;

			int years_to_simulate = nyear_spinup + historic_years;

			double progress = (double)(years_to_simulate + date.year) /
				(double)(years_to_simulate);
			tprogress.setprogress(progress);
			dprintf("%3d%% complete, %s elapsed, %s remaining\n", (int)(progress*100.0),
				tprogress.elapsed.str, tprogress.remaining.str);


			if (verbose)
			{
				double v = -1.0;
				double p = -1.0;

#ifdef WIN32
				PROCESS_MEMORY_COUNTERS_EX pmc;
				GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
				const SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
				const SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
				v = virtualMemUsedByMe / 1024.0 / 1024.0;
				p = physMemUsedByMe / 1024.0 / 1024.0;
#endif

#ifdef linux
				struct sysinfo memInfo;
				sysinfo(&memInfo);
				long long totalVirtualMem = memInfo.totalram;
				//Add other values in next statement to avoid int overflow on right hand side...
				totalVirtualMem += memInfo.totalswap;
				totalVirtualMem *= memInfo.mem_unit;
				int virtualMemUsedByMe = get_virtual_memory();
				int physMemUsedByMe = get_physical_memory();
				v = virtualMemUsedByMe / 1024.0;
				p = physMemUsedByMe / 1024.0;
#endif				
				dprintf(" Virtual memory: %f5.1 mb; Physical memory: %f5.1 mb \n", v, p);
			}




			tmute.settimer(MUTESEC);
		}
	}

	return true;
}

void NcInputParallelAutoSingle::getlandcover(Gridcell& gridcell)
{
	landcover_input.getlandcover(gridcell);
	landcover_input.get_land_transitions(gridcell);
}


double& NcInputParallelAutoSingle::GetInputMs()
{
	return elapsed_input_ms;
}

void NcInputParallelAutoSingle::SendReceiveHandling()
{
	mpi_gridlist_handler->SendReceiving();
}
#endif
#endif