#include "MultipleInsDistributingGridcells.h"
#include <iostream>


MultipleInsDistributingGridcells::MultipleInsDistributingGridcells(const CommandLineArguments& args) :
GuessSystemBuilder(args)
{
}


MultipleInsDistributingGridcells::~MultipleInsDistributingGridcells()
{
}

void MultipleInsDistributingGridcells::run()
{

	if (args.get_input_module_type() == InputModuleType::NCS)
	{
		fprintf(stderr, "Parallel-gridcells-method can not be used with the ncs-input module");
		exit(EXIT_FAILURE);
	}


	const std::vector<std::string>& insfiles = args.get_instruction_files();
	
	for (const std::string& insfile : insfiles)
	{
		Parallel& parallel = Parallel::get();

		std::string ins_without_path_and_ext = remove_extension_and_path(insfile);
		
		if (parallel.is_root())
		{
			create_directory(ins_without_path_and_ext);
			copy_file(insfile, ins_without_path_and_ext + "/" + remove_path(insfile));
		}

		parallel.wait();
		change_directory(ins_without_path_and_ext);


		// Setting up parallel folder structure This has to be done as late as here, 
		// because depending on the type of the simulation the folder structure is deeper
		std::string subpath = get_path(insfile);
		parallel.setup_files_and_folders(remove_path(insfile).c_str());

		// Set our shell for the model to communicate with the world
		set_shell(new CommandLineShell(file_log));

		CommandLineArguments sub_args(args, remove_path(insfile));



		//Root takes control over the simulation but does not simulate LPJ-GUESS
		if (parallel.is_root() && (sub_args.get_input_module_type() == InputModuleType::NCPA))
			control_processes_auto(sub_args);

		else if (parallel.is_root() && (sub_args.get_input_module_type() == InputModuleType::NCPS))
			control_processes_auto_single(sub_args);

		// Every other process except root is doing simulations
		else
			framework(sub_args);




		change_directory(root_path);
	}
}
