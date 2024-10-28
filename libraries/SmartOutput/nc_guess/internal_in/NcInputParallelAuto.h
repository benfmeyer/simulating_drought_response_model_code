//////////////////////////////////////////////////////////////////////////////////////
/// \file NcInputParallelAuto.h
/// \brief Input module where one process misson controls (sends gridcells) to all other processes
/// All processes write to one file using PNetCDF
/// Advantage: Faster output
/// Disadvantage: No NCDF4 only NCDF3
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "NetCDF_Common.h"

#include "inputmodule.h"
#include "internal_in/MPI_Gridlist_Auto.h"
#include <memory>
#include "guessnc.h"
#include "cruinput.h"
#include "ICoordParser.h"
#include "CoordI.h"
#include <fstream>

class NcInputParallelAuto :
	public InputModule, NetCDF_Common
{
public:
	NcInputParallelAuto(InputModuleType type);
	~NcInputParallelAuto();

	void init(const CommandLineArguments& args) override;
	bool getgridcell(Gridcell& gridcell) override;
	bool getclimate(Gridcell& gridcell) override;
	void getlandcover(Gridcell& gridcell) override;
	void getmanagement(Gridcell& gridcell) override { management_input.getmanagement(gridcell); }
	double& GetInputMs();

	void SendReceiveHandling();


private:

	//PP new
	std::unique_ptr<nc_guess_internal::MPI_Gridlist_Auto> mpi_gridlist_handler;

	const std::string global_log_filename = "guess_global.log";
	std::ofstream global_log;
};

