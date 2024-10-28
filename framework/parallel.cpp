#include "parallel.h"
#include "config.h"
#include "shell.h"
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

#include <sys/types.h> // required for stat.h
#include <sys/stat.h> // no clue why required -- man pages say so
#include <sstream>

// platform independent function for changing working directory
// we'll call our new function change_directory
#ifdef _MSC_VER
// The Microsoft way
#include <direct.h>
#define cd _chdir
#else
// The POSIX way
#include <unistd.h>
#include <limits.h>
#define cd chdir
#endif

#ifdef _MSC_VER
#define GetCurrentDir _getcwd
#else
#define GetCurrentDir getcwd
#endif

char cCurrentPath[FILENAME_MAX];


std::string get_working_directory()
{
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		std::cout << "Error getting current directory";
	}

	return std::string(cCurrentPath);
}



void change_directory(std::string path)
{
	int nError = 0;
	nError = cd(path.c_str());
	if (nError != 0)
	{
		std::cout << "Failed changing dir!" << std::endl;
		EXIT_FAILURE;
	}
}

void create_directory(std::string name)
{
	int nError = 0;
#if defined(_WIN32)
	nError = _mkdir(name.c_str()); // can be used on Windows
#else // UNIX style permissions
	mode_t nMode = 0733;
	nError = mkdir(name.c_str(), nMode); // can be used on non-Windows
#endif
	if (nError != 0)
	{
		std::cout << "Failed creating dir! Already exits?" << std::endl;
		EXIT_FAILURE;
	}
}

void copy_file(std::string name, std::string destination)
{
	std::ifstream source(name.c_str(), std::ios::binary);
	std::ofstream dest(destination.c_str(), std::ios::binary);
	std::istreambuf_iterator<char> begin_source(source);
	std::istreambuf_iterator<char> end_source;
	std::ostreambuf_iterator<char> begin_dest(dest);

	copy(begin_source, end_source, begin_dest);

	source.close();
	dest.close();

}

void Parallel::init(int argc, char** argv)
{
	// Only initialize MPI is the command line options explicitly
	// asks for a parallel run (with the -parallel option).
	// In most cases it wouldn't hurt to initialize MPI even if
	// it's not used, but apparently some implementations start
	// by going to the users home directory if the program isn't
	// launched by an mpiexec/mpirun launcher.

	// Unfortunately, since MPI initialization must be done before
	// we parse our options with CommandLineArguments, we need to
	// look for the -parallel option here by ourselves.
	// The file-global variable parallel is initiated = false;
	for (int i = 0; i < argc; ++i) {
		if (std::string(argv[i]) == "-parallel" || std::string(argv[i]) == "-p") {
			parallel = true;
		}
	}

	if (parallel) {
#ifdef HAVE_MPI
		MPI_Init(&argc, &argv);
#endif
	}

}

void Parallel::parse(CommandLineArguments& args)
{
#ifdef HAVE_MPI

	if (parallel) {

		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		input_module = args.get_input_module_type();

		if (rank == ROOT_ID)
			isRoot = true;

		if (input_module == InputModuleType::NCPA || input_module == InputModuleType::NCPS)
		{
			int key = 0;
			int color;

			if (rank == ROOT_ID)
				color = MPI_UNDEFINED;
			else
				color = 0;

			MPI_Comm_split(MPI_COMM_WORLD, color, key, &all_except_root_comm);
		}
	}

#endif

}

int Parallel::get_rank()
{
#ifdef HAVE_MPI
	return rank;
#else
	return 0;
#endif
}

int Parallel::get_num_processes()
{
#ifdef HAVE_MPI
	if (parallel) {
		return size;
	}
	else
		return 1;
#else
	return 1;
#endif
}

bool Parallel::is_root()
{
#ifdef HAVE_MPI
	if (parallel) {

		return isRoot;
	}
	else
		return true;
#else
	return 1;
#endif
}

void Parallel::setup_files_and_folders(const char* filename)
{
	if (parallel)
	{
		if (input_module == InputModuleType::NCPA || input_module == InputModuleType::NCPS)
		{
			if (!isRoot)
			{
				setup_run(filename);
			}
		}

		else
		{
			setup_run(filename);
		}
	}

}

void Parallel::wait()
{
	if (parallel)
	{
#ifdef HAVE_MPI
		MPI_Barrier(MPI_COMM_WORLD);
#endif
	}
}

void Parallel::setup_run(const char* filename)
{
	std::string folder = "run" + std::to_string(rank + 1);
	std::string despath = folder + "/" + filename;

	create_directory(folder);
	copy_file(filename, despath);
	change_directory(folder);
}


#ifdef HAVE_MPI
MPI_Comm& Parallel::get_communicator()
{
	return all_except_root_comm;
}
#endif

Parallel::Parallel(): parallel(false), isRoot(false), rank(0), size(0)
{
}

Parallel::~Parallel()
{
	if (parallel)
	{
#ifdef HAVE_MPI
		/*if (input_module == InputModuleType::NCPA || input_module == InputModuleType::NCPS)
		{
	    	MPI_Comm_free(&all_except_root_comm);
		}*/

		MPI_Finalize();
#endif
	}


}


