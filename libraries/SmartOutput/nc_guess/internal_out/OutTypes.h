#pragma once
#include <string>
#include <stdexcept>

enum class OutWriterType
{
	Daily,
	Monthly,
	Annualy,
	Invalid
};

enum class OutputDimension
{
	One,
	Two,
	Three,
	Invalid
};

/**
 * \brief Time range to chose for SmartOutput
 */
enum class OutputTimeRange
{
	//The standard time of the climate data simulation
	Scenario, 
	//Only the spinup is used for output
	Spinup,
	//Spinup + scenario are out out (basically every time step available is simulated)
	Full,
	//Select the begin and end year yourself
	Custom
};

class NcGuessException : public std::runtime_error
{
public:
	NcGuessException(const std::string what) : runtime_error(what) {}
};