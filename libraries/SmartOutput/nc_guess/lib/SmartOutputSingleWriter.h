#pragma once
#include "SmartOutputBuilder.h"


class SmartOutputSingleWriter : nc_guess::SmartOutputBuilder
{
public:
	SmartOutputSingleWriter();
	~SmartOutputSingleWriter();

	void init(InputModule* input_module, const char* output_directory, const bool verbose);

	/**
	 * \brief 
	 * \param data 
	 * \param first_year 
	 * \param last_year 
	 * \param n_total_gridcells 
	 */
	void Write(double* data, CoordI coord, int first_year, int last_year, int n_total_gridcells);

private:

	bool defined_dims_;
	std::vector<std::unique_ptr<BaseWriter>> writers_;


	int index;
	int id;
	double lon;
	double lat;
    std::string descrip;
};


