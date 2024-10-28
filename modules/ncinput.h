///////////////////////////////////////////////////////////////////////////////////////
/// \file NcInputSingleGuess.h
/// \brief Input module for CF conforming NetCDF files
///
/// \author Joe Siltberg
/// $Date: 2015-11-13 16:25:45 +0100 (Fri, 13 Nov 2015) $
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef LPJ_GUESS_NCInput_H
#define LPJ_GUESS_NCInput_H
#ifdef HAVE_SMARTIO


#include "internal_in/NcInputSingle.h"
#include "internal_in/NcInputParallel.h"
#include "internal_in/NcInputParallelAuto.h"
#include "internal_in/NcInputParallelAutoSingle.h"


#include "cruinput.h"
#include "guessnc.h"
#include <memory>
#include <limits>

class NcInputParallelAutoGuess : public InputModule {
public:
	NcInputParallelAutoGuess(InputModuleType type);

	~NcInputParallelAutoGuess();

	void init(const CommandLineArguments& args) override;

	/// See base class for documentation about this function's responsibilities
	bool getgridcell(Gridcell& gridcell) override;

	/// See base class for documentation about this function's responsibilities
	bool getclimate(Gridcell& gridcell) override;

	/// See base class for documentation about this function's responsibilities
	void getlandcover(Gridcell& gridcell) override;

	/// Obtains land management data for one day
	void getmanagement(Gridcell& gridcell) override;

	void SendReceiveHandling();


	double& GetInputMs();

	static const int NYEAR_SPINUP_DATA = 3000;

private:
	NcInputParallelAuto nc_input_parallel;
};


class NcInputParallelAutoSingleGuess : public InputModule {
public:
	NcInputParallelAutoSingleGuess(InputModuleType type);

	~NcInputParallelAutoSingleGuess();

	void init(const CommandLineArguments& args) override;

	/// See base class for documentation about this function's responsibilities
	bool getgridcell(Gridcell& gridcell) override;

	/// See base class for documentation about this function's responsibilities
	bool getclimate(Gridcell& gridcell) override;

	/// See base class for documentation about this function's responsibilities
	void getlandcover(Gridcell& gridcell) override;

	/// Obtains land management data for one day
	void getmanagement(Gridcell& gridcell) override;

	void SendReceiveHandling();
	
	double& GetInputMs();

	static const int NYEAR_SPINUP_DATA = 3000;

	nc_guess_internal::MPI_Gridlist_Data_Send* Get_MPI_Input();

private:
	NcInputParallelAutoSingle nc_input_parallel;
};


#endif //HAVE_SMARTIO
#endif // LPJ_GUESS_NCInput_H
