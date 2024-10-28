//////////////////////////////////////////////////////////////////////////////////////
/// \file NcInputParallelAutoSingle.h
/// \brief Input module where one process misson controls (sends gridcells) to all other processes
/// The processes send back all data to the process and this one writes the data to one NCDF file
/// Advantage: NCDF4 support
/// Disadvang: Can be slow with daily output
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "NetCDF_Common.h"

#include "inputmodule.h"
#include "internal_in/MPI_Gridlist_Data_Send.h"
#include <memory>
#include "guessnc.h"
#include "cruinput.h"
#include "ICoordParser.h"
#include "CoordI.h"
#include <fstream>

class NcInputParallelAutoSingle :
	public InputModule, NetCDF_Common
{
public:
	NcInputParallelAutoSingle(InputModuleType type);
	~NcInputParallelAutoSingle();

	void init(const CommandLineArguments& args) override;
	bool getgridcell(Gridcell& gridcell) override;
	bool getclimate(Gridcell& gridcell) override;
	void getlandcover(Gridcell& gridcell) override;
	void getmanagement(Gridcell& gridcell) override { management_input.getmanagement(gridcell); }
	double& GetInputMs();

	void SendReceiveHandling();

	// ToDo make this member private and pass a pointer as get
	std::unique_ptr<nc_guess_internal::MPI_Gridlist_Data_Send> mpi_gridlist_handler;

private:

	const std::string global_log_filename = "guess_global.log";
	std::ofstream global_log;

	bool verbose;
	bool load_balancing;
};
