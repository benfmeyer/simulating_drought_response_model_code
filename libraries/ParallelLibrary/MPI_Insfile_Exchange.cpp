#include "MPI_Insfile_Exchange.h"
#include "parallel.h"
#include <chrono>
#include <iostream>
#include "internal_in/MPI_Gridlist_Auto.h"
#include "internal_in/TimeConverter.h"
#include "OutputVariable.h"


MPI_Insfile_Exchange::MPI_Insfile_Exchange(int n_ins_files):n_insfiles(n_ins_files),is_finished_(false)
{
	global_log.open("guess_global.txt");

	Parallel& parallel = Parallel::get();

	rank_ = parallel.get_rank();
	n_processes_ = parallel.get_num_processes();
	root_ = parallel.is_root();

}

MPI_Insfile_Exchange::~MPI_Insfile_Exchange()
{
	global_log.flush();
	global_log.close();

}

void MPI_Insfile_Exchange::SendInitialInsfiles()
{
	if (root_)
	{
		insfile_counts_per_proc.resize(n_processes_ - 1);
		insfiles_send_ = 0;	


		// Initial send:
		// Send each process one gridcell
		// We start at 1 because root does not get any gridlists, he is only the mission control
		
		for (int i = 1; i < n_processes_; ++i)
		{
			int send_buf[2];		

			//if (true)
			//{
			//	std::string m =
			//		"Send: {" + std::to_string(send_buf[0]) + " " +
			//		std::to_string(send_buf[1]) + " " +
			//		std::to_string(send_buf[2]) + " " +
			//		std::to_string(send_buf[3]) + " " +
			//		std::to_string(send_buf[4]) + " } to process " + std::to_string(i);
			//	nc_guess_interal::print_message(m);
			//}



			// The very likely situation:
			// each process will get a gridcell
			if (i < n_insfiles + 1)
			{
				send_buf[0] = 1;
				send_buf[1] = i - 1;
				insfile_counts_per_proc[i - 1]++;
				insfiles_send_ = i;
			}

			// We have the unlikely initialization that we have more processes
			// then gridcells to simulate. In this case just tell those remaining 
			// processes to do nothing
			else
			{
				send_buf[0] = 0;
				send_buf[1] = -1;
			}





			// We could also think of a MPI_Isend() at this point
			// However, MPI_Send() should only be negligable slower
			MPI_Send(send_buf, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

	}

}

void MPI_Insfile_Exchange::SendReceiveHandling()
{

	insfiles_recv_ = 0;
	size_t finished_processes = 0;

	using std::chrono::system_clock;

	system_clock::time_point start_time = system_clock::now();
	RemainingStringGenerator remaing_time_str(start_time, n_insfiles);

	// Loop untile we received all the gridcells and also the success message
	// As root is not looping with all the others we have to substract 1 from the 
	// number of processes
	//while (insfiles_recv_ < n_insfiles  && finished_processes < (n_processes_ - 1) + 1)

	while (finished_processes < (n_processes_ - 1))
	{
		int complete;
		MPI_Status status;

		MPI_Recv(&complete, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		if (!complete)
		{
			insfiles_recv_++;
			insfile_counts_per_proc[status.MPI_SOURCE - 1]++;
		}
		else
		{
			finished_processes++;
		}

		//std::cout << "insfiles_recv: "<< std::to_string(insfiles_recv_) << std::endl;
		//std::cout << "finishedprocesses: "<< std::to_string(finished_processes) << std::endl;
		//std::cout << "process: "<< std::to_string(status.MPI_SOURCE) << std::endl;
		//std::cout << "complete: "<< std::to_string(complete) << std::endl;


		if (!complete)
		{
			std::string message = remaing_time_str.Get(insfiles_recv_);
			global_log << nc_guess_internal::print_message(message) << std::endl;
			global_log.flush();
		}


		//if (verbose_)
		//{
		//	std::string m =
		//		"Received: {" + std::to_string(complete) + "} from process " + std::to_string(status.MPI_SOURCE);
		//	log_file_ << print_message(m) << std::endl;
		//}

		int send_buf[2];

		// Only send "real" gridcells if we have some available in list
		if (insfiles_send_ < n_insfiles)
		{
			send_buf[0] = 1;
			send_buf[1] = insfiles_send_;



			// Increase number of send gridcells.
			insfiles_send_++;
		}

		// No more gridcells remaining
		// Tell those processes to stop.
		else
		{
			send_buf[0] = 0;
			send_buf[1] = -1;
		}
			   

		// Send information back to process.
		// If the process is complete, do not send anything back
		if (complete == 0)
		{
			MPI_Send(send_buf, 2, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD);

			//if (true)
			//{
			//	std::string m =
			//		"Send: {" + std::to_string(send_buf[0]) + " " +
			//		std::to_string(send_buf[1]) + " " +
			//		std::to_string(send_buf[2]) + " " +
			//		std::to_string(send_buf[3]) + " " +
			//		std::to_string(send_buf[4]) + " } to process " + std::to_string(status.MPI_SOURCE);
			//	global_log << m << std::endl;
			//}


		}

	}

	//if (verbose_)
	//{
	//	for (int i = 1; i < n_processes_; ++i)
	//	{
	//		std::string m = "Process " + std::to_string(i) + " has simulated " +
	//			std::to_string(gridcell_counts_per_proc[i - 1])
	//			+ " gridcells.";
	//		log_file_ << print_message(m) << std::endl;

	//	}
	//}

	//Finished Simulation

}

void MPI_Insfile_Exchange::ReceiveNextInsfile()
{
	if (!root_)
	{
		int recv_buf[2];
		MPI_Status status;

		MPI_Recv(recv_buf, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		//if (verbose_)
		//{
		//	std::string m = "Rank " + std::to_string(rank_) +
		//		" received: { " + std::to_string(recv_buf[0]) + " " +
		//		std::to_string(recv_buf[1]) + " " +
		//		std::to_string(recv_buf[2]) + " " +
		//		std::to_string(recv_buf[3]) + " " +
		//		std::to_string(recv_buf[4]) + " } from root.";
		//	std::cout << print_message(m) << std::endl;
		//}

		//std::cout << Parallel::get().get_rank() << " " << recv_buf[1] << std::endl;
		//std::cout << "Done2" << std::endl;


		// First entry of array shows if we have a new coord (1)
		// or if this process should terminate
		// We got a new (good) coord
		if (recv_buf[0] == 1)
			is_finished_ = false;
		else
			is_finished_ = true;

		next_insfile_id = recv_buf[1];
	}
}

void MPI_Insfile_Exchange::SendDoneToRoot()
{
	// We are just sending some arbitarty value to let the root know, that this process is done with the gridcell
	// We can later change this to some more meaningfull values.
	int completly_done;

	if (is_finished_)
		completly_done = 1;
	else
		completly_done = 0;

	MPI_Send(&completly_done, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

bool MPI_Insfile_Exchange::GetIsFinished() const
{
	return is_finished_;
}

int MPI_Insfile_Exchange::GetNextCoord() const
{
	return next_insfile_id;
}
