///////////////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
/// \brief Main module for command line version of LPJ-GUESS
///
/// \author Ben Smith
/// $Date: 2013-07-17 09:22:52 +0200 (Wed, 17 Jul 2013) $
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <stdlib.h>
#include <iostream>
#include "gutil.h"
#include "shell.h"
#include "framework.h"
#include "parallel.h"
#include "commandlinearguments.h"
#include "parameters.h"
#ifdef HAVE_SMARTIO
#include "Model.h"
#endif

#ifdef __unix__
// includes needed for umask()
#include <sys/types.h>
#include <sys/stat.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////
// MAIN
// This is the function called when the executable is run

int main(int argc, char* argv[]) {


#ifdef __unix__

	// On unix systems we set the umask (which controls the file permissions
	// of files we create). Some MPI implementations set a restrictive umask
	// which would mean that other users can't read the files generated by
	// LPJ-GUESS.
	umask(S_IWGRP | S_IWOTH); // only disable write access for group and others

#endif

#ifdef HAVE_SMARTIO
	Model LPJ_GUESS;

	LPJ_GUESS.init(argc, argv);

	LPJ_GUESS.run();
#else
	const char* file_log = "guess.log";

	Parallel& parallel = Parallel::get();

	parallel.init(argc, argv);

	CommandLineArguments args(argc, argv);


	if (args.get_help()) {
		printhelp();
		exit(0);
	}

	// Setting up parallel folder structure This has to be done as late as here, 
	// because depending on the type of the simulation the folder structure is deeper
	parallel.setup_files_and_folders(args.get_instruction_file());

	// Parsing parallel arguments 
	parallel.parse(args);

	// Set our shell for the model to communicate with the world
	set_shell(new CommandLineShell(file_log));

	framework(args);

#endif

	// Say goodbye
	dprintf("\nFinished\n");

	return EXIT_SUCCESS;
}
