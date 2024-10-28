//////////////////////////////////////////////////////////////////////////////////////
/// \file NcInputParallel.h
/// \brief Parallel input module using PNetcdf and an internal gridcell distribution algorithm
/// No mission control process. All processes simulate.
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "NetCDF_Common.h"
#include "internal_in/MPI_Gridlist_Parser.h"
#include <memory>
#include "guessnc.h"
#include "cruinput.h"
#include "ICoordParser.h"
#include "CoordI.h"

class NcInputParallel : public InputModule, NetCDF_Common
{

public:
	NcInputParallel(InputModuleType type);

	~NcInputParallel();

	void init(const CommandLineArguments& args) override;

	/// See base class for documentation about this function's responsibilities
	bool getgridcell(Gridcell& gridcell) override;

	/// See base class for documentation about this function's responsibilities
	bool getclimate(Gridcell& gridcell) override;

	/// See base class for documentation about this function's responsibilities
	void getlandcover(Gridcell& gridcell) override;

	/// Obtains land management data for one day
	void getmanagement(Gridcell& gridcell) override { management_input.getmanagement(gridcell); }

	double& GetInputMs();

private:
	//PP new
	std::unique_ptr<nc_guess_internal::MPI_Gridlist_Parser> mpi_parser_;

	std::vector<CoordI>::iterator current_index_iter;
};

