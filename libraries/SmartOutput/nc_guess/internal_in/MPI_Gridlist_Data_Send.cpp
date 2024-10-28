#include "MPI_Gridlist_Data_Send.h"
#include "MPI_Gridlist_Auto.h"
#include "gutil.h"
#include "parameters.h"
#include "guess.h"
#include "guessstring.h"
#include <fstream>
#include <sstream>
#include <mpi.h>
#include "internal_out/OutTypes.h"
#include "parallel.h"
#include "vector"
#include <iomanip> // put_time
#include <chrono>
#include <ctime>
#include <iostream>
#include "internal_out/BaseWriter.h"
#include "smartoutput.h"
#include "TimeConverter.h"

nc_guess_internal::MPI_Gridlist_Data_Send::MPI_Gridlist_Data_Send(const bool is_reduced, GuessNC::CF::GridcellOrderedVariable& var, std::ofstream& log_file, bool verbose, InputModule* module) :
	is_finished_(false), log_file_(log_file), verbose_(verbose),is_reduced_(is_reduced)
{
	Parallel& parallel = Parallel::get();

	rank_ = parallel.get_rank();
	n_processes_ = parallel.get_num_processes();
	root_ = parallel.is_root();


	if (root_)
	{
		//Obtain sptatial resolution
		GetSpatialResolution(var);

		ReadGridList();

		writer_ = std::make_unique<SmartOutputSingleWriter>();

		writer_->init(module,"", so_options.verbose);
	}

	//Send total number of gridcells (this is needed for collective ncmpi file creation)
	MPI_Bcast(&total_gridcells, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (verbose_ && root_)
		log_file_ << print_message("Broadcast complete!") << std::endl;



}


nc_guess_internal::MPI_Gridlist_Data_Send::~MPI_Gridlist_Data_Send()
{
}

size_t nc_guess_internal::MPI_Gridlist_Data_Send::GetNumberOfGridcells() const
{
	return static_cast<size_t>(total_gridcells);
}

void nc_guess_internal::MPI_Gridlist_Data_Send::SendInitialCoords()
{
	if (root_)
	{
		gridcell_counts_per_proc_.resize(n_processes_ - 1);
		gridcells_send_ = 0;

		// Initial send:
		// Send each process one gridcell
		// We start at 1 because root does not get any gridlists, he is only the mission control

		for (int i = 1; i < n_processes_; ++i)
		{
			int send_buf[5];

			// The very likely situation:
			// each process will get a gridcell
			if (i < total_gridcells)
			{
				send_buf[0] = 1;
				send_buf[1] = gridlist_[i - 1].index;
				send_buf[2] = gridlist_[i - 1].landid;
				send_buf[3] = gridlist_[i - 1].lon_index;
				send_buf[4] = gridlist_[i - 1].lat_index;
				gridcell_counts_per_proc_[i - 1]++;
				gridcells_send_ = i;
	
			}

			// We have the unlikely initialization that we have more processes
			// then gridcells to simulate. In this case just tell those remaining 
			// processes to do nothing
			else
			{
				send_buf[0] = 0;
				send_buf[1] = -1;
				send_buf[2] = -1;
				send_buf[3] = -1;
				send_buf[4] = -1;
			}

			// We could also think of a MPI_Isend() at this point
			// However, MPI_Send() should only be negligable slower
			MPI_Send(send_buf, 5, MPI_INT, i, 0, MPI_COMM_WORLD);

			if (verbose_)
			{
				std::string m =
					"Send: {" + std::to_string(send_buf[0]) + " " +
					std::to_string(send_buf[1]) + " " +
					std::to_string(send_buf[2]) + " " +
					std::to_string(send_buf[3]) + " " +
					std::to_string(send_buf[4]) + " } to process " + std::to_string(i);
				log_file_ << print_message(m) << std::endl;
			}
		}
	}
}

void nc_guess_internal::MPI_Gridlist_Data_Send::SendReceiving()
{
	size_t BUFFERSIZE = 1024 * 1024 * 128;

	gridcells_recv_ = 0;
	size_t finished_processes = 0;

	using std::chrono::system_clock;

	system_clock::time_point start_time = system_clock::now();

	RemainingStringGenerator remaing_time_str(start_time, total_gridcells);

	// Loop untile we received all the gridcells and also the success message
	// As root is not looping with all the others we have to substract 1 from the 
	// number of processes
	while (gridcells_recv_ < total_gridcells  && finished_processes < (n_processes_ - 1))
	{
		int complete;
		MPI_Status status;
		int n_data_points;

		data_ = new double[MAX_BUFFERSIZE];
		
		MPI_Recv(data_, BUFFERSIZE, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_DOUBLE, &n_data_points);
		n_data_length = n_data_points;
		

		complete = static_cast<int>(data_[0]);

		if (complete == 0)
		{
			gridcells_recv_++;
			gridcell_counts_per_proc_[status.MPI_SOURCE - 1]++;



			auto start = std::chrono::high_resolution_clock::now();

			WriteData();

			auto end = std::chrono::high_resolution_clock::now();
			auto writing_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			//log_file_ << print_message << "Time waited and taken for receiving " << std::to_string(writing_ms) << " ms\n";
		}

		

		else
		{
			finished_processes++;
		}


		delete[] data_;




		


		if (!complete)
		{
			std::string message = remaing_time_str.Get(gridcells_recv_);
			log_file_ << print_message(message) << std::endl;
			log_file_.flush();
		}


		if (verbose_)
		{
			std::string m =
				"Received: {" + std::to_string(complete) + "} from process " + std::to_string(status.MPI_SOURCE);
			log_file_ << print_message(m) << std::endl;
		}

		int send_buf[5];

		// Only send "real" gridcells if we have some available in list
		if (gridcells_send_ < total_gridcells)
		{
			send_buf[0] = 1;
			send_buf[1] = gridlist_[gridcells_send_].index;
			send_buf[2] = gridlist_[gridcells_send_].landid;
			send_buf[3] = gridlist_[gridcells_send_].lon_index;
			send_buf[4] = gridlist_[gridcells_send_].lat_index;

			// Increase number of send gridcells.
			gridcells_send_++;
		}

		// No more gridcells remaining
		// Tell those processes to stop.
		else
		{
			send_buf[0] = 0;
			send_buf[1] = -1;
			send_buf[2] = -1;
			send_buf[3] = -1;
			send_buf[4] = -1;
		}


		// Send information back to process.
		// If the process is complete, do not send anything back
		if (complete == 0)
		{
			MPI_Send(send_buf, 5, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD);

			if (verbose_)
			{
				std::string m =
					"Send: {" + std::to_string(send_buf[0]) + " " +
					std::to_string(send_buf[1]) + " " +
					std::to_string(send_buf[2]) + " " +
					std::to_string(send_buf[3]) + " " +
					std::to_string(send_buf[4]) + " } to process " + std::to_string(status.MPI_SOURCE);
				log_file_ << print_message(m) << std::endl;
			}

		}

	}

	std::cout << "Send-Receive-Handling Done!" << std::endl;

	if (verbose_)
	{
		for (int i = 1; i < n_processes_; ++i)
		{
			std::string m = "Process " + std::to_string(i) + " has simulated " +
				std::to_string(gridcell_counts_per_proc_[i - 1])
				+ " gridcells.";
			log_file_ << print_message(m) << std::endl;

		}
	}

	//Finished Simulation
}

void nc_guess_internal::MPI_Gridlist_Data_Send::WriteData()
{
	//std::string m = "This data has index " + std::to_string(data_[1]);
	//log_file_ << print_message(m)<< std::endl;

	int simulated_coord_index = data_[1];
	CoordI coord = gridlist_[simulated_coord_index];

	bool successfull = data_[2] == 0.0 ? false : true;

	coord.succes = successfull;

	writer_->Write(data_, coord, first_year ,last_year, total_gridcells);
}

void nc_guess_internal::MPI_Gridlist_Data_Send::ReceiveNextCoord()
{
	if (!root_)
	{
		if (verbose_)
			std::cout << print_message("Receiving coords.. ");

		int recv_buf[5];
		MPI_Status status;

		MPI_Recv(recv_buf, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		if (verbose_)
		{
			std::string m = "Rank " + std::to_string(rank_) +
				" received: { " + std::to_string(recv_buf[0]) + " " +
				std::to_string(recv_buf[1]) + " " +
				std::to_string(recv_buf[2]) + " " +
				std::to_string(recv_buf[3]) + " " +
				std::to_string(recv_buf[4]) + " } from root.";
			std::cout << print_message(m) << std::endl;
		}


		// First entry of array shows if we have a new coord (1)
		// or if this process should terminate
		// We got a new (good) coord
		if (recv_buf[0] == 1)
			is_finished_ = false;
		else
			is_finished_ = true;
		

		next_coord_ = {};
		next_coord_.index = recv_buf[1];
		next_coord_.landid = recv_buf[2];
		next_coord_.lon_index = recv_buf[3];
		next_coord_.lat_index = recv_buf[4];
	}
}

void nc_guess_internal::MPI_Gridlist_Data_Send::SendData(const std::vector<double>& data) const
{

	// We are just sending some arbitarty value to let the root know, that this process is done with the gridcell
	// We can later change this to some more meaningfull values.
	int completly_done;

	if (is_finished_)
		completly_done = 1;
	else
		completly_done = 0;
	
	// We are also sending the status next to the data (first entry)
	size_t n_data = data.size() + 5;	
	
	double* data_send = new double[n_data];


	data_send[0] = completly_done;
	data_send[1] = next_coord_.index;

	// When this function is beeing called, OutAnnual is also beeing called.
	// This means the simulation was sucessfull!
	data_send[2] = 1.0;

	// Copy the sending data to the data package.
	for (int i = 0; i < data.size(); ++i)
		data_send[i + 3] = data[i];
	
	MPI_Send(data_send, n_data, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

	delete[] data_send;

	std::string m = "Process" + std::to_string(rank_) +
		" send: {" + std::to_string(completly_done) + "} to root.";
	std::cout << print_message(m) << std::endl;


}

void nc_guess_internal::MPI_Gridlist_Data_Send::ReadGridList()
{
	// Retrieve name of grid list file as read from ins file
	xtring file_gridlist = param["file_gridlist"].str;

	int index = 0;

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

		c.index = index;
		gridlist_.push_back(c);
		index++;
	}

	

	total_gridcells = gridlist_.size();

	//"Free" memory
	coords.clear();
}

void nc_guess_internal::MPI_Gridlist_Data_Send::SendDoneToRoot(bool successfull) const
{
	 //We are just sending some arbitarty value to let the root know, that this process is done with the gridcell
	 //We can later change this to some more meaningfull values.
	double completly_done;

	if (is_finished_)
		completly_done = 1;
	else
		completly_done = 0;


	double send_buf[3];
	send_buf[0] = completly_done;
	send_buf[1] = next_coord_.index;

	if (!successfull)
		send_buf[2] = 0.0;
	else
		send_buf[2] = 1.0;


	MPI_Send(send_buf, 3, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

	std::string m = "Rank " + std::to_string(rank_) +
		" send: {" + std::to_string(completly_done) + "} to root.";
	std::cout << print_message(m) << std::endl;

}

bool nc_guess_internal::MPI_Gridlist_Data_Send::GetIsFinished() const
{
	return is_finished_;
}

CoordI nc_guess_internal::MPI_Gridlist_Data_Send::GetNextCoord() const
{
	return next_coord_;
}
