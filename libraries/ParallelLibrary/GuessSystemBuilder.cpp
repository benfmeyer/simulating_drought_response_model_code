#include "GuessSystemBuilder.h"
//#include <experimental/filesystem>


GuessSystemBuilder::GuessSystemBuilder(const CommandLineArguments& args): args(args)
{
	root_path = get_working_directory();
}

GuessSystemBuilder::~GuessSystemBuilder()
{
}

std::string GuessSystemBuilder::remove_extension(const std::string& filename)
{
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

std::string GuessSystemBuilder::remove_path(const std::string& filename)
{
	std::string new_filename;

	size_t lastslash = filename.find_last_of("/");
	if (lastslash == std::string::npos)
	{
		size_t last_two_backs_slash = filename.find_last_of("\\");
		if (last_two_backs_slash == std::string::npos)
		{
			return filename;
		}

		return new_filename.assign(filename.begin() + last_two_backs_slash + 1, filename.end());
	}

	return new_filename.assign(filename.begin() + lastslash + 1, filename.end());
	
}

std::string GuessSystemBuilder::remove_extension_and_path(const std::string& filename)
{

	std::string  s_without_path = remove_path(filename);
	return remove_extension(s_without_path);
}

std::string GuessSystemBuilder::get_path(const std::string& filename)
{
	std::string new_filename;

	size_t lastslash = filename.find_last_of("/");
	if (lastslash == std::string::npos)
	{
		size_t last_two_backs_slash = filename.find_last_of("\\");
		if (last_two_backs_slash == std::string::npos)
		{
			return filename;
		}

		return new_filename.assign(filename.begin() ,filename.begin() + last_two_backs_slash);
	}

	return new_filename.assign(filename.begin(), filename.begin() + lastslash);
}
