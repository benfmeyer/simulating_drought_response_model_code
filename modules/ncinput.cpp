///////////////////////////////////////////////////////////////////////////////////////
/// \file NCInput.cpp
/// \brief Input module for CF conforming NetCDF files
///
/// \author Joe Siltberg
/// $Date: 2015-12-14 16:08:55 +0100 (Mon, 14 Dec 2015) $
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "ncinput.h"

#ifdef HAVE_SMARTIO

#include "guess.h"
#include <sstream>

using namespace GuessNC::CF;

#ifdef HAVE_MPI
NcInputParallelAutoGuess::NcInputParallelAutoGuess(InputModuleType type) : InputModule(type), nc_input_parallel(type)
{
}

NcInputParallelAutoGuess::~NcInputParallelAutoGuess()
{
}

void NcInputParallelAutoGuess::init(const CommandLineArguments& args)
{
	nc_input_parallel.init(args);
}

bool NcInputParallelAutoGuess::getgridcell(Gridcell& gridcell)
{
	return nc_input_parallel.getgridcell(gridcell);
}
void NcInputParallelAutoGuess::getlandcover(Gridcell& gridcell)
{
	nc_input_parallel.getlandcover(gridcell);
}

void NcInputParallelAutoGuess::getmanagement(Gridcell& gridcell)
{
	nc_input_parallel.getmanagement(gridcell);
}

void NcInputParallelAutoGuess::SendReceiveHandling()
{
	nc_input_parallel.SendReceiveHandling();
}

double& NcInputParallelAutoGuess::GetInputMs()
{
	return nc_input_parallel.GetInputMs();
}

bool NcInputParallelAutoGuess::getclimate(Gridcell& gridcell)
{
	return nc_input_parallel.getclimate(gridcell);
}




NcInputParallelAutoSingleGuess::NcInputParallelAutoSingleGuess(InputModuleType type) : InputModule(type), nc_input_parallel(type)
{
}

NcInputParallelAutoSingleGuess::~NcInputParallelAutoSingleGuess()
{
}

void NcInputParallelAutoSingleGuess::init(const CommandLineArguments& args)
{
	nc_input_parallel.init(args);
}

bool NcInputParallelAutoSingleGuess::getgridcell(Gridcell& gridcell)
{
	return nc_input_parallel.getgridcell(gridcell);
}
void NcInputParallelAutoSingleGuess::getlandcover(Gridcell& gridcell)
{
	nc_input_parallel.getlandcover(gridcell);
}

void NcInputParallelAutoSingleGuess::getmanagement(Gridcell& gridcell)
{
	nc_input_parallel.getmanagement(gridcell);
}

void NcInputParallelAutoSingleGuess::SendReceiveHandling()
{
	nc_input_parallel.SendReceiveHandling();
}

double& NcInputParallelAutoSingleGuess::GetInputMs()
{
	return nc_input_parallel.GetInputMs();
}

nc_guess_internal::MPI_Gridlist_Data_Send* NcInputParallelAutoSingleGuess::Get_MPI_Input()
{
	return nc_input_parallel.mpi_gridlist_handler.get();
}

bool NcInputParallelAutoSingleGuess::getclimate(Gridcell& gridcell)
{
	return nc_input_parallel.getclimate(gridcell);
}


#endif // HAVE_MPI
#endif // HAVE_NETCDF
