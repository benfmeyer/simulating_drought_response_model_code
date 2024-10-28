///////////////////////////////////////////////////////////////////////////////////////
/// \file inputmodule.h
/// \brief Base class for input modules
///
/// \author Joe Siltberg
/// $Date: 2019-04-23 14:48:43 +0200 (Tue, 23 Apr 2019) $
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef LPJ_GUESS_INPUT_MODULE_H
#define LPJ_GUESS_INPUT_MODULE_H

#include <map>
#include <string>
#include "commandlinearguments.h"
#include <memory>

class Gridcell;

/// Base class from which any input module must inherit
/** An input module supplies LPJ-GUESS with the forcing data it needs. The
 *  InputModule base class is an abstract class, meaning it only defines
 *  the interface which subclasses must implement.
 *
 *  To create a new input module, create a new class which inherits from
 *  this one, implement all the member functions below, and register it
 *  with the REGISTER_INPUT_MODULE macro (defined and documented below).
 */
class InputModule {
public:
	explicit InputModule(InputModuleType type)
		: type(type) {}

	/// Called after the instruction file has been read
	/** Initialises the input module (e.g. opening files). Typically
	 *  reads in a gridlist.
	 */
	virtual void init(const CommandLineArguments& args) = 0;

	/// Obtains coordinates and soil static parameters for the next grid cell to simulate 
	/** The function should return false if no grid cells remain to be simulated,
	 *  otherwise true. 
	 *
	 *  If the model is to be driven by quasi-daily values of the climate variables
	 *  derived from monthly means, this function may be the appropriate place to
	 *  perform the required interpolations. The utility functions interp_monthly_means_conserve
	 *  and interp_monthly_totals_conserve in driver.cpp may be called for this purpose.
	 */
	virtual bool getgridcell(Gridcell& gridcell) = 0;

	/// Obtains climate data (including atmospheric CO2 and insolation) for this day
	/** The function should return false if the simulation is complete for this grid cell,
	 *  otherwise true. This will normally require querying the year and day member
	 *  variables of the global class object date:
	 *
	 *  if (date.day==0 && date.year==nyear_spinup) return false;
	 *  // else
	 *  return true;
	 *
	 *  Currently the following member variables of the climate member of gridcell must be
	 *  initialised: co2, temp, prec, insol. If the model is to be driven by quasi-daily
	 *  values of the climate variables derived from monthly means, this day's values
	 *  will presumably be extracted from arrays containing the interpolated daily
	 *  values (see function getgridcell):
	 *
	 *  gridcell.climate.temp=dtemp[date.day];
	 *  gridcell.climate.prec=dprec[date.day];
	 *  gridcell.climate.insol=dsun[date.day];
	 *
	 *  Diurnal temperature range (dtr) added for calculation of leaf temperatures in 
	 *  BVOC:
	 *  gridcell.climate.dtr=ddtr[date.day]; 
	 *
	 *  If model is run in diurnal mode, which requires appropriate climate forcing data, 
	 *  additional members of the climate must be initialised: temps, insols. Both of the
	 *  variables must be of type std::vector. The length of these vectors should be equal
	 *  to value of date.subdaily which also needs to be set either in getclimate or 
	 *  getgridcell functions. date.subdaily is a number of sub-daily period in a single 
	 *  day. Irrespective of the BVOC settings, climate.dtr variable is not required in 
	 *  diurnal mode.
	 */
	virtual bool getclimate(Gridcell& gridcell) = 0;

	/// Obtains land transitions for one year
	virtual void getlandcover(Gridcell& gridcell) = 0;

	/// Obtains land management data for one day
	virtual void getmanagement(Gridcell& gridcell) = 0;

	InputModuleType GetType();

private:
	InputModuleType type;
};



class InputModuleRegistry2
{
public:
	static std::unique_ptr<InputModule> CreateInstance(InputModuleType type);
};



#endif // LPJ_GUESS_INPUT_MODULE_H
