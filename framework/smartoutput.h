///////////////////////////////////////////////////////////////////////////////////////
/// \file SmartOutput.h
/// \brief Smartoutput Maincode. This should be the only file which connects
/// the LPJ-GUESS maincode with NC_GUESS. For a description, see manual
/// This file should not be touched under normal circumtances
/// To introduce new variables use the SmartOutputBuilder class
/// \author Phillip Papastefanou
/// $Date: 2017-02-07 23:17:02 +0100 (Wed, 07 Feb 2017) $
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "outputmodule.h"
#include <chrono>
#ifdef HAVE_SMARTIO
#include "lib/SmartOutputBuilder.h"
#include "internal_in/MPI_Gridlist_Data_Send.h"

const size_t SMART_OUT_MAJOR = 1;
const size_t SMART_OUT_MINOR = 4;
const size_t SMART_OUT_REVISION = 1;
const size_t SMART_OUT_BUILD = 0;

class Patch;
class Stand;
class Pft;

namespace GuessOutput {

	using namespace out_guess;

	class SmartOutput : public OutputModule, public nc_guess::SmartOutputBuilder
	{
	public:
		SmartOutput();
		~SmartOutput();
		// implemented functions inherited from OutputModule (see documentation in OutputModule)
		void init(InputModule* input_module, const char* output_directory, const bool verbose) override;
		// implemented functions inherited from OutputModule (see documentation in OutputModule)
		void outannual(Gridcell& gridcell) override;
		// implemented functions inherited from OutputModule (see documentation in OutputModule)
		void outdaily(Gridcell& gridcell) override;
		void openlocalfiles(Gridcell& gridcell) override;
		void closelocalfiles(Gridcell& gridcell) override;

	private:

		#ifdef HAVE_MPI
		nc_guess_internal::MPI_Gridlist_Data_Send* sender_;
		#endif


		std::chrono::high_resolution_clock::time_point start;
		double elapsed_ms;
		double writing_ms;
		double* input_ms;

		/// active fraction of all cohorts of the same pft type for this gridcell
		double active_fraction_;

		/// have the NetCDF-Writer already left define mode
		bool defined_dims_;

		/// currently selected standpft (deprecated or not in use)
		Standpft* selected_standpft_;

		/// currently selected patchpft (deprecated or not in use)
		Patchpft* selected_patchpft_;

		/// References to NetCDF writers. Current versions supports up to three writers (annualy, monthly, daily)
		vector<std::reference_wrapper<BaseWriter>> writers;

		/// Check if one output type hase been overwritten
		void CheckWritersForSupression();

		/// Gets the active fraction per pft and gridcell
		void GetActiveFraction(Gridcell& gridcell, Pft& pft);
		
		///Loops through all pfts (deprecated, not in use, only for active fraction calculation)
		void PftLoop(Gridcell& gridcell);
		
		/// Loops through all stands per gridcell
		void StandLoop(Gridcell& gridcell);

		/// Loops through all patches per stand
		void PatchLoop(Stand& stand);

		/// Loops through all cohorts/indivs per patch
		void IndivLoop(Stand& stand, Patch& patch);

		/// Count the alive indivuals
		void CountAliveIndividuals(Patch& patch);

		void CountAliveIndiviualsAcrossAllPatches(Stand& stand);


	};

}
#endif
