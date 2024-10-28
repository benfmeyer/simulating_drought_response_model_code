#pragma once
#include <memory>
#ifdef HAVE_MPI
#include <limits.h>
#include <vector>
#include <fstream>


class MPI_Insfile_Exchange
{
public:
	MPI_Insfile_Exchange(int n_ins_files);
	~MPI_Insfile_Exchange();

	void SendInitialInsfiles();
	void SendReceiveHandling();
	void ReceiveNextInsfile();
	void SendDoneToRoot();

	/// Is this process finished ?! 
	bool GetIsFinished() const;

	int GetNextCoord() const;

private:

	int rank_;
	int n_processes_;
	int insfiles_per_process_;
	int n_insfiles;

	bool root_;
	bool is_finished_;

	size_t insfiles_send_;
	size_t insfiles_recv_;

	std::vector<int> insfile_counts_per_proc;

	int next_insfile_id;

	std::ofstream global_log;
};

#endif