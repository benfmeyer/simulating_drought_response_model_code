#include "StandardGuess.h"
#include <iostream>


StandardGuess::StandardGuess(const CommandLineArguments& args): GuessSystemBuilder(args)
{
}

StandardGuess::~StandardGuess()
= default;

void StandardGuess::run()
{
	Parallel& parallel = Parallel::get();

	// Setting up parallel folder structure This has to be done as late as here, 
	// because depending on the type of the simulation the folder structure is deeper
	parallel.setup_files_and_folders(args.get_instruction_file());

	// Set our shell for the model to communicate with the world
	set_shell(new CommandLineShell(file_log));

	//Root takes control over the simulation but does not simulate LPJ-GUESS
	if (parallel.is_root() && (args.get_input_module_type() == InputModuleType::NCPA))
		control_processes_auto(args);

	else if (parallel.is_root() && (args.get_input_module_type() == InputModuleType::NCPS))
		control_processes_auto_single(args);

	// Every other process except root is doing simulations
	else
		framework(args);
}
