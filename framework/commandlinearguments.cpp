///////////////////////////////////////////////////////////////////////////////////////
/// \file commandlinearguments.cpp
/// \brief Takes care of the command line arguments to LPJ-GUESS
///
/// $Date: 2016-12-08 18:24:04 +0100 (Thu, 08 Dec 2016) $
///
///////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "commandlinearguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>
#include <fstream>

namespace {

std::string tolower(const char* str) {
	std::string result(str);
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

}

CommandLineArguments::CommandLineArguments(int argc, char** argv) 
: help(false),
  parallel(false),
	verbose(false), 
load_balancing(false),
use_multi_insfiles_distribute_gridcells(false),
use_multi_insfiles_distribute_insfiles(false),
  input_module_string("cru_ncep") {

	driver_file = "";

	if (!parse_arguments(argc, argv)) {
		print_usage(argv[0]);
	}
}


CommandLineArguments::CommandLineArguments(const CommandLineArguments& main_arguments, const std::string& insfile):
help(false),
parallel(main_arguments.parallel),
verbose(main_arguments.verbose),
load_balancing(main_arguments.verbose),
use_multi_insfiles_distribute_gridcells(main_arguments.use_multi_insfiles_distribute_gridcells),
use_multi_insfiles_distribute_insfiles(main_arguments.use_multi_insfiles_distribute_insfiles),
input_module_string(main_arguments.input_module_string),
input_module_type(main_arguments.input_module_type),
insfile(insfile)
{

}

bool CommandLineArguments::parse_arguments(int argc, char** argv) {
	if (argc < 2) {
		return false;
	}

	// For now, just consider anything starting with '-' to be an option,
	// and anything else to be the ins file
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			std::string option = tolower(argv[i]);
			if (option == "-help") {
				help = true;
			}
			else if (option == "-parallel" || option == "-p") {
				parallel = true;	
			}
			else if (option == "-v"|| option == "-verbose") {
				verbose = true;
			}
			else if (option == "-lb" || option == "-loadbalancing") {
				load_balancing = true;
			}
			else if (option == "-mdg"){
				use_multi_insfiles_distribute_gridcells = true;
			}
			else if (option == "-mdi") {
				use_multi_insfiles_distribute_insfiles = true;
			}

			else if (option == "-input") {
				if (i+1 < argc) {
					std::string module = tolower(argv[i + 1]);
					if (module == "getclim") {
						// If GetClim input module requested, next argument should be path to driver file
						if (i + 2 < argc){
							input_module_string = argv[i + 1];
							driver_file = argv[i + 2];
							i += 2; // skip two arguments
						}
						else {
							fprintf(stderr, "Missing pathname after -input getclim");
							return false;
						}
					}
					else {
						input_module_string = argv[i + 1];
						++i; // skip the next argument
					}
				}
				else {
					fprintf(stderr, "Missing argument after -input\n");
					return false;
				}
			}
			else {
				fprintf(stderr, "Unknown option: \"%s\"\n", argv[i]);
				return false;
			}
		}
		else {
			if (insfile.empty()) {
				insfile = argv[i];
			}
			else {
				fprintf(stderr, "Two arguments parsed as ins file: %s and %s\n",
						  insfile.c_str(), argv[i]);
				return false;
			}
		}
	}

	// The only time it's ok not to specify an insfile is if -help is used
	if (insfile.empty() && !help) {
		fprintf(stderr, "No instruction file specified\n");
		return false;
	}
	

	parse_input_modules();

	
	if (use_multi_insfiles_distribute_gridcells)	{
		read_insfile_list();
	}

	if (use_multi_insfiles_distribute_insfiles) {
		read_insfile_list();
	}

	return true;
}

void CommandLineArguments::print_usage(const char* command_name) const {
	fprintf(stderr, "\nUsage: %s [-parallel] [-input <module_name> [<GetClim-driver-file-path>] ] <instruction-script-filename> | -help\n", 
			  command_name);
	exit(EXIT_FAILURE);
}

void CommandLineArguments::parse_input_modules()
{
	if (input_module_string == "cf")
		input_module_type = InputModuleType::CF;
	else if (input_module_string == "getclim")
		input_module_type = InputModuleType::GETCLIM;
	else if (input_module_string == "cru_ncep")
		input_module_type = InputModuleType::CRU_NCEP;
	else if (input_module_string == "ncs")
		input_module_type = InputModuleType::NCS;
	else if (input_module_string == "ncp")
		input_module_type = InputModuleType::NCP;
	else if (input_module_string == "ncps")
		input_module_type = InputModuleType::NCPS;
	else if (input_module_string == "ncpa")
		input_module_type = InputModuleType::NCPA;
	else
	{
		fprintf(stderr, "Invalid input module");
		exit(EXIT_FAILURE);
	}
}

void CommandLineArguments::read_insfile_list()
{
	insfiles.clear();

	std::ifstream ifs(insfile, std::ifstream::in);

	if (!ifs.good())	{
		fprintf(stderr, "Could not open %s for as input file list", insfile.c_str());
		exit(EXIT_FAILURE);
	}
	
	std::string line;
	while (getline(ifs, line)) {
		if (!line.empty()) {
			insfiles.push_back(line);
		}	
	}	
}

bool CommandLineArguments::get_help() const {
	return help;
}

bool CommandLineArguments::get_parallel() const {
	return parallel;
}

bool CommandLineArguments::get_verbose() const
{
	return verbose;
}

bool CommandLineArguments::get_load_balancing() const
{
	return load_balancing;
}


const char* CommandLineArguments::get_instruction_file() const {
	return insfile.c_str();
}

const std::string& CommandLineArguments::get_input_module_string() const {
	return input_module_string;
}

const InputModuleType& CommandLineArguments::get_input_module_type() const
{
	return input_module_type;
}

const char* CommandLineArguments::get_driver_file() const {
	return driver_file.c_str();
}

bool CommandLineArguments::get_use_multi_ins_dist_gridcells() const
{
	return use_multi_insfiles_distribute_gridcells;
}

bool CommandLineArguments::get_use_multi_ins_dist_insfiles() const
{
	return use_multi_insfiles_distribute_insfiles;
}

const std::vector<std::string>& CommandLineArguments::get_instruction_files() const
{
	return insfiles;
}
