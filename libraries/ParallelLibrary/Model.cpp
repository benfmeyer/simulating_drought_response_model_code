#include "Model.h"
#include "parallel.h"
#include "config.h"
#include "guess.h"
#include "framework.h"
#include "commandlinearguments.h"
#include <stdlib.h>
#include <iostream>
#include "StandardGuess.h"
#include "MultipleInsDistributingGridcells.h"
#include "MultipleInsDistributingInsfiles.h"


Model::Model()
{

}

Model::~Model()
{

}

void Model::init(int argc, char** argv)
{
	Parallel& parallel = Parallel::get();

	parallel.init(argc, argv);

	args = std::make_unique<CommandLineArguments>(argc, argv);

	if (args->get_help()) {
		printhelp();
		exit(0);
	}

	// Parsing parallel arguments 
	parallel.parse(*args);
}

void Model::run()
{
	if (args->get_use_multi_ins_dist_gridcells())
		model = std::make_unique<MultipleInsDistributingGridcells>(*args);

	else if(args->get_use_multi_ins_dist_insfiles())
		model = std::make_unique<MultipleInsDistributingInsfiles>(*args);

	else
		model = std::make_unique<StandardGuess>(*args);

	model->run();
}
