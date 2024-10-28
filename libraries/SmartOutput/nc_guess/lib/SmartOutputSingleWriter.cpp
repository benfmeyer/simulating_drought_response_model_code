#include "SmartOutputSingleWriter.h"
#include <chrono>

SmartOutputSingleWriter::SmartOutputSingleWriter():SmartOutputBuilder(so_options), defined_dims_(false)
{
}

SmartOutputSingleWriter::~SmartOutputSingleWriter()
{

	for (std::unique_ptr<BaseWriter>& writer : writers_)
		writer->Close();
}

void SmartOutputSingleWriter::init(InputModule* input_module, const char* output_directory, const bool verbose)
{
	using namespace out_guess;


	if (!opts.supress_annualy_output)
		writers_.push_back(std::make_unique<SingleWriterAnnualy>(so_options, output_directory));
	if (!opts.supress_monthly_output)
		writers_.push_back(std::make_unique<SingleWriterMonthly>(so_options, output_directory));
	if (!opts.supress_daily_output)
		writers_.push_back(std::make_unique<SingleWriterDaily>(so_options, output_directory));

		
	for (std::unique_ptr<BaseWriter>& writer : writers_)
		writer->Init();
	

	Define();

	for (std::unique_ptr<BaseWriter>& writer : writers_)
		writer->ConnectToPatchCollector(&stand_patch_collectors_);	

	
	for (std::unique_ptr<BaseWriter>& writer : writers_)
		writer->ConnectToPftCollector(&stand_patch_pft_collectors_);


}

void SmartOutputSingleWriter::Write(double* data, CoordI coord,  int first_year, int last_year, int n_total_gridcells)
{
	
	//if (!defined_dims_)
	//{		
	//	annualWriter->SetTimeBeginEnd(first_year, last_year);
	//	annualWriter->SetProcesses(1, n_total_gridcells);
	//	annualWriter->SetTimeSteps();
	//	annualWriter->DefineDims();
	//	annualWriter->DefineBaseVariables();
	//	annualWriter->DefineVariables();
	//	annualWriter->WriteTime();
	//	annualWriter->WritePfts();

	//	defined_dims_ = true;
	//}

	//
	//index = coord.index;
	//id = coord.landid;
	//lon = coord.lon;
	//lat = coord.lat;


	//annualWriter->SetStationIndexAndID(index, id);
	//annualWriter->SetCurrentLonLat(lon, lat);

	//
	//annualWriter->WriteStationID();
	//annualWriter->WriteLonLat();

	//int offset = 5;
	//if (coord.succes)
	//	annualWriter->WriteAllVariablesSingle(data, offset);


	if (!defined_dims_)
	{
		for (std::unique_ptr<BaseWriter>& writer : writers_)
		{
			writer->SetTimeBeginEnd(first_year, last_year);
			writer->SetProcesses(1, n_total_gridcells);
			writer->SetTimeSteps();
			writer->DefineDims();
			writer->DefineBaseVariables();
			writer->DefineVariables();
			writer->WriteTime();
			writer->WritePfts();
			writer->WriteStands();
		}	

		defined_dims_ = true;
	}



	index = coord.index;
	id = coord.landid;
	lon = coord.lon;
	lat = coord.lat;
    descrip = coord.descrip;


	for (std::unique_ptr<BaseWriter>& writer : writers_)
	{
		writer->SetStationIndexAndID(index, id);
		writer->SetCurrentLonLat(lon, lat);
        writer->SetCurrentStationName(descrip);
		writer->WriteStationID();
		writer->WriteLonLat();
        writer->WriteStationName();
	}
	

	int offset = 3;
	auto start = std::chrono::high_resolution_clock::now();
	
	for (std::unique_ptr<BaseWriter>& writer : writers_)
	{
		if (coord.succes)
		{
			writer->WriteAllVariablesSingle(data, offset);			
		}			
	}

	auto end = std::chrono::high_resolution_clock::now();

	auto writing_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::string s = "Time waited and taken for receiving " + std::to_string(writing_ms) + " ms\n";
	dprintf(s.c_str());


}
