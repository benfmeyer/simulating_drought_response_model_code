//////////////////////////////////////////////////////////////////////////////////////
/// \file MPI_Gridlist_Data_Send.h
/// \brief Send recieve algorithms for gridlist and data exchange 
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include "lib/SmartOutputSingleWriter.h"
#include <limits.h>
#include <vector>
#include "ICoordParser.h"
#include "cfvariable.h"
#include "CoordI.h"


namespace nc_guess_internal
{


	class MPI_Gridlist_Data_Send : public ICoordParser
	{
	public:
		MPI_Gridlist_Data_Send(const bool is_reduced, GuessNC::CF::GridcellOrderedVariable& var, std::ofstream& outputfile, bool verbose, InputModule* module);
		~MPI_Gridlist_Data_Send();

		size_t GetNumberOfGridcells() const;

		void SendInitialCoords();

		void SendReceiving();
		
		void WriteData();

		void SendDoneToRoot(bool successfull) const;

		/// Is this process finished ?! 
		bool GetIsFinished() const;

		CoordI GetNextCoord() const;

		void ReceiveNextCoord();

		void SendData(const std::vector<double>& data) const;

		int first_year;
		int last_year;


	private:
		double* data_;
		int n_data_length;
		const size_t MAX_BUFFERSIZE = 32 * 1024 * 1024;


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

		std::vector<int> gridcell_counts_per_proc_;
		std::unique_ptr<SmartOutputSingleWriter> writer_;






		void ReadGridList();
	};
}
