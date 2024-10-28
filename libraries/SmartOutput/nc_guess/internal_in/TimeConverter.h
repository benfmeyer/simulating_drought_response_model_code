//////////////////////////////////////////////////////////////////////////////////////
/// \file TimeConverter.h
/// \brief Auxiliary method to convert seconds into h-mm-ss
/// 
/// \author Phillip Papastefanou
/// $Date: 2017-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <chrono>

class TotalSecondConverter
{
public:
	TotalSecondConverter(const int seconds_total) :seconds(0), minuits(0), hours(0)
	{
		int seconds_remaining = seconds_total;

		while (seconds_remaining > 0)
		{
			if (seconds_remaining > 3600)
			{
				hours = seconds_remaining / 3600;
				seconds_remaining -= hours * 3600;
			}

			else if (seconds_remaining > 60)
			{
				minuits = seconds_remaining / 60;
				seconds_remaining -= minuits * 60;
			}
			else
			{
				seconds = seconds_remaining;
				seconds_remaining = 0;
			}
		}


		if (seconds < 10)
			seconds_s = "0" + std::to_string(seconds);
		else
			seconds_s = std::to_string(seconds);

		if (minuits < 10)
			minuits_s = "0" + std::to_string(minuits);
		else
			minuits_s = std::to_string(minuits);

		if (hours < 10)
			hours_s = "0" + std::to_string(hours);
		else
			hours_s = std::to_string(hours);


	}

	std::string seconds_s;
	std::string minuits_s;
	std::string hours_s;


	int seconds;
	int minuits;
	int hours;
};


class RemainingStringGenerator
{
public:
	RemainingStringGenerator(const std::chrono::system_clock::time_point& start_time, int total_gridcells);
	std::string Get(int gridcells_recv);

	private:
	const std::chrono::system_clock::time_point& start_time;
	int total_gridcells;
	

};




