#include "MultipleInsDistributingInsfiles.h"
#include "MPI_Insfile_Exchange.h"
#include <iostream>

MultipleInsDistributingInsfiles::MultipleInsDistributingInsfiles(const CommandLineArguments& args) :
	GuessSystemBuilder(args)
{
}

MultipleInsDistributingInsfiles::~MultipleInsDistributingInsfiles()
{
}

void MultipleInsDistributingInsfiles::run()
{
	if (args.get_input_module_type() != InputModuleType::NCS)
	{
		fprintf(stderr, "Parallel-insfiles-method can only be used with the ncs-input module");
		exit(EXIT_FAILURE);
	}
	
	const std::vector<std::string>& insfiles = args.get_instruction_files();

	MPI_Insfile_Exchange insfiles_exchange(insfiles.size());


	insfiles_exchange.SendInitialInsfiles();
	
	if (!Parallel::get().is_root())
	{
		while (true)
		{
			insfiles_exchange.ReceiveNextInsfile();

			if (insfiles_exchange.GetIsFinished())
			{
				insfiles_exchange.SendDoneToRoot();
				break;
			}

			int id = insfiles_exchange.GetNextCoord();
			const std::string& insfile = insfiles[id];


			std::string ins_without_path_and_ext = remove_extension_and_path(insfile);
						
			create_directory(ins_without_path_and_ext);
			copy_file(insfile, ins_without_path_and_ext + "/" + remove_path(insfile));
			change_directory(ins_without_path_and_ext);

			// Set our shell for the model to communicate with the world
			set_shell(new CommandLineShell(file_log));

			CommandLineArguments sub_args(args, remove_path(insfile));

			framework(sub_args);

			change_directory(root_path);

			insfiles_exchange.SendDoneToRoot();

		}
	}

	else
	{
		// Set our shell for the model to communicate with the world
		set_shell(new CommandLineShell(file_log));

		insfiles_exchange.SendReceiveHandling();
	}
}
