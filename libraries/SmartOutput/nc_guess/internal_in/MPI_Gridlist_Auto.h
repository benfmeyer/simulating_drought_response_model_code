//////////////////////////////////////////////////////////////////////////////////////
/// \file MPI_Gridlist_Auto.h
/// \brief Gridlist distribution across all processes using all available nodes for computation
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#ifdef HAVE_MPI

#include <limits.h>
#include <vector>
#include "ICoordParser.h"
#include "cfvariable.h"
#include "CoordI.h"

namespace nc_guess_internal
{
	std::string get_current_time_stamp();
	std::string print_message(std::string message);


	class MPI_Gridlist_Auto : public ICoordParser
	{
	public:
		MPI_Gridlist_Auto(const bool is_reduced, GuessNC::CF::GridcellOrderedVariable& var, std::ofstream& outputfile, bool verbose);
		~MPI_Gridlist_Auto();

		size_t GetNumberOfGridcells() const;

		void SendInitialCoords();

		void SendReceiveHandling();

		void ReceiveNextCoord();

		void SendDoneToRoot() const;
		
		/// Is this process finished ?! 
		bool GetIsFinished() const;
		
		CoordI GetNextCoord() const;


	private:
		int rank_;
		int n_processes_;
		int gridcells_per_process_;
		int total_gridcells;

		bool root_;
		bool is_reduced_;


		bool is_finished_;
		CoordI next_coord_;

		std::ofstream& log_file_;
		bool verbose_;

		std::vector<CoordI> gridlist_;


		size_t gridcells_send_;
		size_t gridcells_recv_;

		std::vector<int> gridcell_counts_per_proc;


		void ReadGridList();
	};
}



#endif
