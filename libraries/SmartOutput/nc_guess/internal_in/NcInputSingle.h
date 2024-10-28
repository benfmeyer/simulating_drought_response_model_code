///////////////////////////////////////////////////////////////////////////////////////
/// \file NcInputSingle.h
/// \brief Input module for CF conforming NetCDF files 
///  Designed for one task, one insfile writing to one file and no parallelism 
///  Can however also be used using multiple ins files
///
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef LPJ_GUESS_NCInput2_H
#define LPJ_GUESS_NCInput2_H
#include "ICoordParser.h"
#include "CoordI.h"
#include "NetCDF_Common.h"

#include "cruinput.h"
#include "guessnc.h"
#include <memory>
#include <limits>
#include <chrono>

class NcInputSingle : public InputModule, public ICoordParser, NetCDF_Common {
public:
	
	NcInputSingle(InputModuleType type);
	~NcInputSingle();

	void init(const CommandLineArguments& args) override;

	/// See base class for documentation about this function's responsibilities
	bool getgridcell(Gridcell& gridcell) override;

	/// See base class for documentation about this function's responsibilities
	bool getclimate(Gridcell& gridcell) override;
	
	/// See base class for documentation about this function's responsibilities
	void getlandcover(Gridcell& gridcell) override;

	/// Obtains land management data for one day
	void getmanagement(Gridcell& gridcell) override {management_input.getmanagement(gridcell);}

	double& GetInputMs();

private:
	std::vector<CoordI>::iterator current_index_iter;
};

#endif // LPJ_GUESS_NCInput2_H
