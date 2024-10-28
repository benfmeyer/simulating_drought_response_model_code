// MPI implementataion according to Singleton design pattern
#pragma once
#include <string>
#include "commandlinearguments.h"
// mpi.h needs to be the first include (or specifically, before stdio.h),
// due to a bug in the MPI-2 standard (both stdio.h and the C++ MPI API
// defines SEEK_SET, SEEK_CUR and SEEK_SET(!))
#ifdef HAVE_MPI
#include <mpi.h>
#endif

std::string get_working_directory();
void change_directory(std::string path);
void create_directory(std::string name);
void copy_file(std::string name, std::string destination);


class Parallel
{
public:

	static Parallel& get()
	{
		static Parallel instance; 

		return instance;
	}

	Parallel(Parallel const&) = delete;
	void operator=(Parallel const&) = delete;


	/// Initializes the module
	/** For instance by initializing underlying communications library.
	*
	*  \param argc Passed along from main() to underlying communications library
	*  \param argv Passed along from main() to underlying communications library
	*/
	void init(int argc, char** argv);

	// Todo Write description
	void parse(CommandLineArguments& args);

	void setup_files_and_folders(const char* insfile);

	void wait();

	/// The process id of this process in the parallel run
	/** Returns zero when no MPI library is available/used. */
	int get_rank();

	/// The number of processes involved in the parallel run
	/** Returns 1 when no MPI library is available/used. */
	int get_num_processes();

	/// Determines if process is root
	/** Returns true when no MPI library is available/used. */
	bool is_root();


#ifdef HAVE_MPI
	MPI_Comm& get_communicator();
#endif


private:
	Parallel();
	~Parallel();

	void setup_run(const char* filename);

	const int ROOT_ID = 0;
	bool parallel;
	bool isRoot;
	int rank;
	int size;
	InputModuleType input_module;
#ifdef HAVE_MPI
	MPI_Comm all_except_root_comm;
#endif // HAVE_MPI

};
