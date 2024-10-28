#pragma once
#include "commandlinearguments.h"
#include "gutil.h"
#include "framework.h"
#include "parallel.h"
#include "shell.h"

class GuessSystemBuilder
{
public:
	GuessSystemBuilder(const CommandLineArguments& args);
	virtual ~GuessSystemBuilder();

	virtual void run() = 0;
protected:
	const CommandLineArguments& args;
	// LOG FILE
	// The name of the log file to which output from all dprintf and fail calls is sent is
	// set here
	xtring file_log = "guess.log";
	std::string root_path;
	std::string remove_extension(const std::string& filename);
	std::string remove_path(const std::string& filename);
	std::string remove_extension_and_path(const std::string& filename);
	std::string get_path(const std::string& filename);

};

