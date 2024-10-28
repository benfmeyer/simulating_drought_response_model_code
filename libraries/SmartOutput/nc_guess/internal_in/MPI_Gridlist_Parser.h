//////////////////////////////////////////////////////////////////////////////////////
/// \file MPI_Gridlist_Parser.h
/// \brief Parsing class on how to distribute gridcells across multiple nodes 
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>

#include <limits.h>
#include <vector>
#include "ICoordParser.h"
#include "cfvariable.h"
#include "CoordI.h"
#include "IGridcellDistributionSystem.h"


namespace nc_guess_internal
{
	class MPI_Gridlist_Parser : public ICoordParser
	{

	public:

		MPI_Gridlist_Parser(const bool is_reduced, const bool load_balancing, GuessNC::CF::GridcellOrderedVariable& var);
		~MPI_Gridlist_Parser();

		std::vector<CoordI> GetGridcellsPerProcess() const;
		size_t GetNumberOfGridcells() const;

	private:
		int rank_;
		int n_processes_;
		int gridcells_per_process_;
		int total_gridcells;

		bool root_;
		bool is_reduced_;
		bool load_balancing_;

		std::unique_ptr<IGridcellDistributionSystem> distribution_system_;


		int* complete_grid;
		int* complete_indicies;

		//Todo Temporary
		int* complete_lon_indexes;
		int* complete_lat_indexes;

		int* sub_grid_arr_;
		int* sub_grid_indicies_;

		//Todo Temporary
		int* sub_lon_indexes;
		int* sub_lat_indexes;

		std::vector<CoordI> gridlist_;
 		std::vector<CoordI> sub_gridlist_;

		void InitializeDistributionSystem();

		void ReadGridList();
		void ReadLoadList();



		void CheckForInvalidCell();
		void GetNumberOfGridcellsPerProcess();

		void Send();

		void GetNumberOfGridcellsPerProcessSkippingRoot();
		void SendSkippingRoot();
	};
}



