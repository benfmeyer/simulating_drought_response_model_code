#include "TimeConverter.h"
#include <iostream>

RemainingStringGenerator::RemainingStringGenerator(const std::chrono::system_clock::time_point& start_time, int total_gridcells):start_time(start_time), total_gridcells(total_gridcells)
{
}

std::string RemainingStringGenerator::Get(int gridcells_recv)
{
	std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
	
	long sconds_elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
	long seconds_remaining = sconds_elapsed * static_cast<double>(total_gridcells) / static_cast<double>(gridcells_recv) - sconds_elapsed;
	
	TotalSecondConverter t_e(sconds_elapsed);
	TotalSecondConverter t_r(seconds_remaining);

	std::string total_gridcells_str = std::to_string(total_gridcells);
	std::string recv_gridcells_str = std::to_string(gridcells_recv);
	std::string recv_gridcells_str_cor = std::string(total_gridcells_str.size() - recv_gridcells_str.length(), '0') + recv_gridcells_str;
	std::string m = "Received " + recv_gridcells_str_cor + " out of " + total_gridcells_str + ".";

	std::string m2 = "Elapsed time: " + t_e.hours_s + ":"
		+ t_e.minuits_s + ":" + t_e.seconds_s + ".";
	std::string m3 = "Remaining time: " + t_r.hours_s + ":"
		+ t_r.minuits_s + ":" + t_r.seconds_s + ".";

	return m + " " + m2 + " " + m3;
}
