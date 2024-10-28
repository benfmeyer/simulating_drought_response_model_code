///////////////////////////////////////////////////////////////////////////////////////
/// \file commandlinearguments.h
/// \brief Takes care of the command line arguments to LPJ-GUESS
///
/// $Date: 2016-12-08 18:24:04 +0100 (Thu, 08 Dec 2016) $
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef LPJ_GUESS_COMMAND_LINE_ARGUMENTS_H
#define LPJ_GUESS_COMMAND_LINE_ARGUMENTS_H

#include <string>
#include <vector>

enum class InputModuleType
{
	DEMO,
	CF,
	GETCLIM,
	CRU_NCEP, 
	NCS, 
	NCP, 
	NCPS,
	NCPA
};

/// Parses and stores the user's command line arguments
class CommandLineArguments {
public:
	/// Send in the arguments from main() to this constructor
	/** If the arguments are malformed, this constructor will
	 *  print out usage information and exit the program.
	 */
	CommandLineArguments(int argc, char** argv);

	///Todo add some nice description here
	CommandLineArguments(const CommandLineArguments& main_arguments, const std::string& insfile);

	/// Returns the instruction filename
	const char* get_instruction_file() const;

	/// Returns true if the user has specified the help option
	bool get_help() const;

	/// Returns true if the user has specified the parallel option
	bool get_parallel() const;

	/// Returns true if the user has specified the verbose option
	bool get_verbose() const;

	/// Return true if user has specified the load_balancing option
	bool get_load_balancing() const;

	/// Returns the chosen (or default) input module string representation
	const std::string& get_input_module_string() const;

	const InputModuleType& get_input_module_type() const;

	/// Returns path to a GetClim-generated driver file if the 'getclim' input module was requested
	const char* get_driver_file() const;

	/// Todo
	bool get_use_multi_ins_dist_gridcells() const;
	/// Todo
	bool get_use_multi_ins_dist_insfiles() const;

	/// Returns the instruction filename
	const std::vector<std::string>& get_instruction_files() const;
	

private:
	/// Does the actual parsing of the arguments
	bool parse_arguments(int argc, char** argv);

	/// Prints out usage information and exits the program
	void print_usage(const char* command_name) const;

	void parse_input_modules();

	void read_insfile_list();

	/// Instruction filename specified on the command line
	std::string insfile;

	/// List of instruction files
	std::vector<std::string> insfiles;

	/// Whether the user wants help on how to run the LPJ-GUESS command
	bool help;

	/// Whether the user requested a parallel run
	bool parallel;

	/// Whether the user requested verbose output
	bool verbose;

	/// Whether to use load balancing, if applicable in the gridcell files
	bool load_balancing;

	/// Todo
	bool use_multi_insfiles_distribute_gridcells;

	///Todo
	bool use_multi_insfiles_distribute_insfiles;

	/// The chosen (or default) input module string representation
	std::string input_module_string;


	InputModuleType input_module_type;

	/// Driver file name for GetClim input module
	std::string driver_file;
};

#endif // LPJ_GUESS_COMMAND_LINE_ARGUMENTS_H
