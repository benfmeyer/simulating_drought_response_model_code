#ifdef HAVE_MPI
#include "MPI_Gridlist_Parser.h"
#include "gutil.h"
#include "parameters.h"
#include "guess.h"
#include "guessstring.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <mpi.h>
#include "internal_out/OutTypes.h"
#include "parallel.h"
#include "vector"

namespace nc_guess_internal
{
	MPI_Gridlist_Parser::MPI_Gridlist_Parser(const bool is_reduced, const bool load_balancing, GuessNC::CF::GridcellOrderedVariable& var) 
	:total_gridcells(0),
	complete_grid(nullptr),
	load_balancing_(load_balancing),
	is_reduced_(is_reduced)
	{
		//if (!is_reduced)
		//	throw NcGuessException("ncp input module only works with reduced input at the current state");

		Parallel& parallel = Parallel::get();

		rank_ = parallel.get_rank();
		n_processes_ = parallel.get_num_processes();
		root_ = parallel.is_root();
		
		/// Must been called after number of processes is known
		InitializeDistributionSystem();				

		if (root_)
		{
			//Obtain sptatial resolution
			GetSpatialResolution(var);

			ReadGridList();

			//Must been called after obtaining gridlist
			if (load_balancing)
				ReadLoadList();
			
			CheckForInvalidCell();

			GetNumberOfGridcellsPerProcess();
		}

		Send();
	}

	std::vector<CoordI> MPI_Gridlist_Parser::GetGridcellsPerProcess() const
	{
		return sub_gridlist_;
	}

	size_t MPI_Gridlist_Parser::GetNumberOfGridcells() const
	{
		return static_cast<size_t>(total_gridcells);
	}

	MPI_Gridlist_Parser::~MPI_Gridlist_Parser()
	{

	}

	void MPI_Gridlist_Parser::InitializeDistributionSystem()
	{
		if (load_balancing_)
			distribution_system_ = std::make_unique<AdaptiveGreedy>(n_processes_);			
		else
			distribution_system_ = std::make_unique<DeckOfCardShuffeling>(n_processes_);
	}

	void MPI_Gridlist_Parser::ReadGridList()
	{
		// Retrieve name of grid list file as read from ins file
		xtring file_gridlist = param["file_gridlist"].str;

		std::ifstream ifs(file_gridlist, std::ifstream::in);
		if (!ifs.good()) fail("NCInputParallel::init: could not open %s for input", (char*)file_gridlist);

		std::string line;
		while (getline(ifs, line)) {

			// Read next record in file
			double rlat, rlon;
			std::string descrip;
			CoordI c;

			std::istringstream iss(line);

			if (iss >> rlon >> rlat)
			{
				getline(iss, descrip);
				c.lat = rlat;
				c.lon = rlon;
			}


			c.descrip = trim(descrip);

			if (is_reduced_)
				c.landid = FindCoordIndex(c.lon, c.lat);
			else
			{
				c.lon_index = FindLonIndex(c.lon);
				c.lat_index = FindLatIndex(c.lat);
			}
			
			gridlist_.push_back(c);
		}

		total_gridcells = gridlist_.size();

		//"Free" memory
		coords.clear();
	}

	void MPI_Gridlist_Parser::ReadLoadList()
	{
		// Retrieve name of grid list file as read from ins file
		xtring file_loadlist = param["file_loadlist"].str;

		std::ifstream ifs(file_loadlist, std::ifstream::in);
		if (!ifs.good()) fail("NCInputParallel::init: could not open %s for load-input", (char*)file_loadlist);

		std::string line;
		int index = 0;
		while (getline(ifs, line)) 
		{
			CoordI& c = gridlist_[index];
			double load = 1.0;
			std::string descrip;
			std::istringstream iss(line);

			if (iss >> load)
			{
				getline(iss, descrip);
				c.load = load;
			}

			index++;
		}

		if (index != total_gridcells)
			fail("Number of gridcells does not match number of loads supplied by load list.");
	}

	void MPI_Gridlist_Parser::CheckForInvalidCell()
	{
		if (is_reduced_)
		{
			//Check if INVALID_CELL is part of the grid_list (extremly unlikely), if yes another invalid cell has to be chosen
			std::vector<CoordI>::iterator iter = std::find_if(gridlist_.begin(), gridlist_.end(), [&](CoordI const& obj) {return obj.landid == IGridcellDistributionSystem::INVALID_GRICELL; });
			if (iter != gridlist_.end())
				throw NcGuessException("You have encounterd the very unlikely case that your gridlist contains a cell-id which serves as invalid cell, please contact papa@tum.de.");
		}
	}

	void MPI_Gridlist_Parser::GetNumberOfGridcellsPerProcess()
	{
		// Resize load vector to have the size of total gridcells
		std::vector<double> loads(total_gridcells);
		if (load_balancing_)
		{
			for (int i = 0; i < total_gridcells; ++i)
			{
				CoordI& coord = gridlist_[i];
				loads[i] = coord.load;
			}
		}
		
		
		std::vector<int> indexes_list;

		try
		{
			indexes_list = distribution_system_->DistributeAsList(loads);
			gridcells_per_process_ = distribution_system_->GetMaximumGridcellsPerNode();
		}
		catch (const std::exception& ex)
		{
			fail(ex.what());
		}
			

		const size_t total_gridcells = indexes_list.size();

		complete_grid = new int[total_gridcells];
		complete_indicies = new int[total_gridcells];
		//Todo Temporary
		complete_lon_indexes = new int[total_gridcells];
		complete_lat_indexes = new int[total_gridcells];


		//Initialise with invalid cell number
		for (int i = 0; i < total_gridcells; ++i)
		{
			complete_grid[i] = IGridcellDistributionSystem::INVALID_GRICELL;
			complete_lon_indexes[i] = IGridcellDistributionSystem::INVALID_GRICELL;
			complete_lat_indexes[i] = IGridcellDistributionSystem::INVALID_GRICELL;
			complete_indicies[i] = -1;
		}		


		for (int i = 0; i < total_gridcells; ++i)
		{
			const int index = indexes_list[i];

			if (index != IGridcellDistributionSystem::INVALID_GRICELL)
			{
				complete_grid[i] = gridlist_[index].landid;
				complete_indicies[i] = index;
				//Todo Temporary
				complete_lon_indexes[i] = gridlist_[index].lon_index;
				complete_lat_indexes[i] = gridlist_[index].lat_index;
			}

		}

	}

	void MPI_Gridlist_Parser::GetNumberOfGridcellsPerProcessSkippingRoot()
	{
		// Step 1
		// First get all the number of gridcells per Process. Most likely there might be processes which have more 
		// gridcells then others, however mpi_scatter only works with rectangular arrays.

		size_t number_of_gridcells = gridlist_.size();


		size_t n_processes_wr = n_processes_ - 1;

		//We increase the number of gridcells_per_process_, so that we have a rectangular array in any case.
		gridcells_per_process_ = number_of_gridcells / n_processes_wr + 1;

		// Step 2
		// Increase number of gridcells and add gridcell-counts with dummy gridcells for root
		size_t total_gridcells = gridcells_per_process_* n_processes_wr + gridcells_per_process_;

		complete_grid = new int[total_gridcells];
		complete_indicies = new int[total_gridcells];

		//Initialise with invalid cell number
		for (int i = 0; i < total_gridcells; ++i)
		{
			complete_grid[i] = IGridcellDistributionSystem::INVALID_GRICELL;
			complete_indicies[i] = -1;
		}


		//Set the first gridlist_per_process values to 
		for (size_t i = 0; i < gridcells_per_process_; i++)
		{
			complete_grid[i] = gridlist_[0].landid;
			complete_indicies[i] = 0;
		}


		//Append shuffle algorithm do the grid
		int c_i = gridcells_per_process_;
		for (int p = 0; p < n_processes_wr; ++p)
		{
			for (int c_p = 0; c_p < number_of_gridcells / n_processes_wr + 1; ++c_p)
			{
				int index = n_processes_wr * c_p + p;
				if (index < number_of_gridcells)
				{
					complete_grid[c_i] = gridlist_[index].landid;
					complete_indicies[c_i] = index;
				}
				c_i++;
			}
		}
	}

	void MPI_Gridlist_Parser::SendSkippingRoot()
	{
		//Send number of gridcells per process and initialse array with that number
		MPI_Bcast(&gridcells_per_process_, 1, MPI_INT, 0, MPI_COMM_WORLD);
		sub_grid_arr_ = new int[gridcells_per_process_];
		sub_grid_indicies_ = new int[gridcells_per_process_];

		//Apply shuffle gridcells IDs among processes
		MPI_Scatter(complete_grid, gridcells_per_process_, MPI_INT, sub_grid_arr_, gridcells_per_process_, MPI_INT, 0, MPI_COMM_WORLD);
	}

	void MPI_Gridlist_Parser::Send()
	{
		//Send total number of gridcells (this is needed for collective ncmpi file creation
		MPI_Bcast(&total_gridcells, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//Send number of gridcells per process and initialse array with that number
		MPI_Bcast(&gridcells_per_process_, 1, MPI_INT, 0, MPI_COMM_WORLD);
		sub_grid_arr_ = new int[gridcells_per_process_];
		sub_grid_indicies_ = new int[gridcells_per_process_];

		//Todo Temporary
		sub_lon_indexes = new int[gridcells_per_process_];
		sub_lat_indexes = new int[gridcells_per_process_];

		//Apply shuffle gridcells IDs among processes
		MPI_Scatter(complete_grid, gridcells_per_process_, MPI_INT, sub_grid_arr_, gridcells_per_process_, MPI_INT, 0, MPI_COMM_WORLD);

		//Apply shuffle gridcells indicies among processes
		MPI_Scatter(complete_indicies, gridcells_per_process_, MPI_INT, sub_grid_indicies_, gridcells_per_process_, MPI_INT, 0, MPI_COMM_WORLD);

		//Todo Temporary
		//Apply shuffle gridcells lons among processes
		MPI_Scatter(complete_lon_indexes, gridcells_per_process_, MPI_INT, sub_lon_indexes, gridcells_per_process_, MPI_INT, 0, MPI_COMM_WORLD);

		//Todo Temporary
		//Apply shuffle gridcells lons among processes
		MPI_Scatter(complete_lat_indexes, gridcells_per_process_, MPI_INT, sub_lat_indexes, gridcells_per_process_, MPI_INT, 0, MPI_COMM_WORLD);

		//Transform grid_array to list and delete invalid cells.
		for (size_t i = 0; i < gridcells_per_process_; ++i)
		{
			if (is_reduced_)
			{
				if (sub_grid_arr_[i] != IGridcellDistributionSystem::INVALID_GRICELL)
				{
					CoordI c;
					c.landid = sub_grid_arr_[i];
					c.index = sub_grid_indicies_[i];
					sub_gridlist_.push_back(c);
				}
			}

			else
			{
				if (sub_lon_indexes[i] != IGridcellDistributionSystem::INVALID_GRICELL)
				{
					CoordI c;
					c.lon_index = sub_lon_indexes[i];
					c.lat_index = sub_lat_indexes[i];
					c.index = sub_grid_indicies_[i];
					sub_gridlist_.push_back(c);
				}
			}
		}

		//Free allocated memory memory
		delete[] sub_grid_arr_;
		delete[] sub_grid_indicies_;
		delete[] sub_lon_indexes;
		delete[] sub_lat_indexes;

		if (root_)
		{
			delete[] complete_grid;
			delete[] complete_indicies;
			delete[] complete_lon_indexes;
			delete[] complete_lat_indexes;
		}
		
	}
}
#endif

