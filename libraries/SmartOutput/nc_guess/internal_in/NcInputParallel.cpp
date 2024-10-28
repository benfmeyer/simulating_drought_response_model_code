#include "NcInputParallel.h"
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

NcInputParallel::NcInputParallel(InputModuleType type):InputModule(type)
{

}

NcInputParallel::~NcInputParallel() {

}

void NcInputParallel::init(const CommandLineArguments& args) {

	this->verbose = args.get_verbose();
	this->load_balancing = args.get_load_balancing();

	init_driver_files();

	check_driver_files();

	// Gridlist are distributed via MPI_Gridlist_parser
	mpi_parser_ = std::make_unique<nc_guess_internal::MPI_Gridlist_Parser>(is_reduced_, load_balancing , *cf_temp);
	gridlist = mpi_parser_->GetGridcellsPerProcess();

	//Display information about gridcells and ids for each process
	if (verbose)
	{
		if (Parallel::get().is_root())
		{
			std::cout << "Rank:" << Parallel::get().get_rank() << "is ROOT and has the following indices: ";
		}

		else
		{
			std::cout << "Rank:" << Parallel::get().get_rank() << "has the following indices: ";
		}

		for (int i = 0; i < gridlist.size(); ++i)
		{
			std::cout << "Index:  " << gridlist[i].index << " with ID: " << gridlist[i].landid << "\t";
		}
		std::cout << std::endl;
	}

	//PP
	first_simulation_year = cf_temp->get_date_time(0).get_year();
	last_simulation_year = cf_temp->get_date_time(cf_temp->get_timesteps() - 1).get_year();
	n_gridcells = gridlist.size();

	current_gridcell = gridlist.front();
	current_index_iter = gridlist.begin();

	init_landcover_and_management();

	date.set_first_calendar_year(cf_temp->get_date_time(0).get_year() - nyear_spinup);

	soilinput.init(param["file_soildata"].str);

	// Set timers
	tprogress.init();
	tmute.init();

	tprogress.settimer();
	tmute.settimer(MUTESEC);
}

bool NcInputParallel::getgridcell(Gridcell& gridcell) {

	double lon, lat;
	double cru_lon, cru_lat;
	int soilcode;

	// Load data for next gridcell, or if that fails, skip ahead until
	// we find one that works.
	while (current_index_iter != gridlist.end() &&
		!load_data_from_files(
			current_index_iter->lon,
			current_index_iter->lat)) {
		++current_index_iter;
		current_gridcell = *current_index_iter;
	}

	if (current_index_iter == gridlist.end()) {
		// simulation finished
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

	gridcell.set_coordinates(lon, lat);
	//PP
	gridcell.set_index(current_index_iter->index);
	gridcell.set_id(current_index_iter->landid);
	gridcell.set_total_counts(n_gridcells, mpi_parser_->GetNumberOfGridcells());
	gridcell.set_time_begin_end(first_simulation_year, last_simulation_year);



	// Load spinup data for all variables
	load_spinup_data(cf_temp, spinup_temp);
	load_spinup_data(cf_prec, spinup_prec);
	load_spinup_data(cf_insol, spinup_insol);

	// --> PPX-Hydraulics
	if (cf_vpd) {
		load_spinup_data(cf_vpd, spinup_vpd);
	}
	else	{
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

	gridcell.climate.instype = cf_standard_name_to_insoltype(cf_insol->get_standard_name());


	soilinput.get_soil(lon, lat, gridcell);
	
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

	dprintf("\nCommencing simulation for gridcell at (%g,%g)\n", lon, lat);
	if (current_index_iter->descrip != "") {
		dprintf("Description: %s\n", current_index_iter->descrip.c_str());
	}
	dprintf("Using soil code and Nitrogen deposition for (%3.1f,%3.1f)\n", cru_lon, cru_lat);

	return true;
}

bool NcInputParallel::getclimate(Gridcell& gridcell) {

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
		++current_index_iter;

		if (current_index_iter != gridlist.end())
			current_gridcell = *current_index_iter;

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

			int cells_done = distance(gridlist.begin(), current_index_iter);

			double progress = (double)(cells_done*years_to_simulate + date.year) /
				(double)(gridlist.size()*years_to_simulate);
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


void NcInputParallel::getlandcover(Gridcell& gridcell) {
	landcover_input.getlandcover(gridcell);
	landcover_input.get_land_transitions(gridcell);
}

double& NcInputParallel::GetInputMs()
{
	return elapsed_input_ms;
}
#endif
#endif
