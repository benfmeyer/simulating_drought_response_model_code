#include "BaseWriter.h"
#include "gutil.h"
#include <sstream>
#include <ctime>
#include "parallel.h"
//#include "lib/SmartOutput.h"
#include "ArrayV3D.h"
#include "internal_in/MPI_Gridlist_Data_Send.h"


BaseWriter::BaseWriter(SmartOutputBuilderOptions& opts, const std::string output_directory) :
opts_(opts),
current_station_id_(0),
current_station_index_(-1),
wrote_time_(true),
wrote_pfts_(true),
patchgroup_("Patch-Out"),
patchpftgroup_("Pft-Out"),
output_directory_(""),
input_module_type(opts.input_module_type),
data_format_(opts.data_format)

{
	const size_t lastCharIndex = output_directory.size() - 1;
	if (lastCharIndex  !=  -1)
	{
		if (output_directory[lastCharIndex] == '/')
			output_directory_ = output_directory;
		else
			output_directory_ =	output_directory + '/';
	}

}

BaseWriter::~BaseWriter()
{
}

void BaseWriter::Init()
{
	using std::to_string;

	#ifdef HAVE_PNETCDF
	writerPNC_ = dynamic_cast<WriterPNC*>(writer_.get());
	#else
	writerNC_ = dynamic_cast<WriterNC*>(writer_.get());
	#endif

	maingroup_.name = "Base";
	//We cannot use unixtime from gutil here as the processes will get different seconds depending on creating time
	//unixtime(timeString);
	time_t t = time(0);
	struct tm * now = localtime(&t);

	std::stringstream ss;
	const string month = now->tm_mon + 1 < 10 ? "0" + to_string(now->tm_mon + 1) : to_string(now->tm_mon + 1);
	const string day = now->tm_mday < 10 ? "0" + to_string(now->tm_mday) : to_string(now->tm_mday);
	ss << "File generated on "
		<< (now->tm_year) + 1900 << "-"
		<< month << "-"
		<< day << " "
		<< (now->tm_hour) << "h";

	Attribute generatedTime;
	generatedTime.name = "Time";
	generatedTime.value = ss.str();
	generatedTime.type = nc_internal::DataType::TEXT;

	Attribute smart_output_version;
	smart_output_version.name = "SmartOutputVersion";
	//smart_output_version.value = std::string("v" +
	//				to_string(SMART_OUT_MAJOR) + "."+
	//				to_string(SMART_OUT_MINOR)+ "." +
	//				to_string(SMART_OUT_REVISION) + "." +
	//				to_string(SMART_OUT_BUILD));
	smart_output_version.type = nc_internal::DataType::TEXT;


	maingroup_.global_attributes.push_back(generatedTime);
	maingroup_.global_attributes.push_back(smart_output_version);

	writer_->AddGroup(patchgroup_);
	writer_->AddGroup(patchpftgroup_);


	//if (GuessParallel::get_rank() == 0)
	//{
	//	wrote_pfts_ = false;
	//	wrote_time_ = false;
	//}
	//All other processes do not have to write time and pfts
	//else
	//{
	//	wrote_pfts_ = true;
	//	wrote_time_ = true;
	//}

	wrote_pfts_ = false;
	wrote_time_ = false;
}


void BaseWriter::ConnectToPatchCollector(std::vector<out_guess::PatchCollector>* stand_patch_collector)
{
	stand_patch_collectors_ = stand_patch_collector;
}

void BaseWriter::ConnectToPftCollector(std::vector<out_guess::PatchPftCollector>* stand_patch_pft_collector)
{
	stand_patch_pft_collectors_ = stand_patch_pft_collector;
}

void BaseWriter::SetProcesses(const size_t n_process_gridcells,	const size_t n_total_gridcells)
{
	n_proc_gridcells_ = n_process_gridcells;
	n_total_gridcells_ = n_total_gridcells;

	//Todo temporary because of full buffered
	// Figure out more clever strategies

	if (writer_type_ == WriterType::Parallel && opts_.input_module_type == InputModuleType::NCPA)
	{
		n_proc_gridcells_ = n_total_gridcells / (Parallel::get().get_num_processes() - 1) + 10 + 0.1*Parallel::get().get_num_processes();
	}
}

void BaseWriter::DefineDims()
{
	vector<Dimension> dims(5);

	Dimension gridcell;
	gridcell.name = "gridcell";
	gridcell.count = n_total_gridcells_;

	Dimension time;
	time.name = "time";
	time.count = n_timesteps_;

	Dimension nstands;
	nstands.name = "stands";
	nstands.count = nst;

	Dimension patches;
	patches.name = "patches";
	patches.count = npatch;

	Dimension pfts;
	pfts.name = "pfts_and_total";
	pfts.count = npft + 1;

	dimensions_.push_back(gridcell);	//dim id 0
	dimensions_.push_back(time);		//dim id 1
	dimensions_.push_back(patches);		//dim id 2
	dimensions_.push_back(pfts);		//dim id 3
	dimensions_.push_back(nstands);		//dim id 4

	//Push dimensions to writer, they are first written, when writer.Define is called
	writer_->SetDimensions(dimensions_);
}

void BaseWriter::DefineBaseVariables()
{
	n_ncvars_ = 0;

	Variable station(maingroup_);
	station.name = "Station";
	station.dim_indicies = { 0 };
	station.type = nc_internal::DataType::INT;

	Variable time(maingroup_);
	time.name = "Time";
	time.dim_indicies = { 1 };
	time.type = nc_internal::DataType::INT;

	Attribute timeAtt;
	timeAtt.name = "units";
	timeAtt.type = nc_internal::DataType::TEXT;
	//Todo double check this
	timeAtt.value = time_unit_ + " since 01-01-" + std::to_string(year_begin_);
	time.attributes.push_back(timeAtt);

    Attribute calendarAtt;
    calendarAtt.name = "calendar";
    calendarAtt.type = nc_internal::DataType::TEXT;
    calendarAtt.value = "365_day";
    time.attributes.push_back(calendarAtt);

	Variable pfts(maingroup_);
	pfts.name = "Pfts";
	pfts.dim_indicies = { 3 };
	pfts.type = nc_internal::DataType::TEXT;


	Variable stands(maingroup_);
	stands.name = "Stands";
	stands.dim_indicies = { 4 };
	stands.type = nc_internal::DataType::TEXT;
	
	//If we have a parallel writer we cannot use strings to store the pfts in the variable
	//we simply store them as attributes of Pft variable
	if (writer_type_ == WriterType::Parallel)
		AddPftAttributes(pfts);

	Variable longitude(maingroup_);
	longitude.name = "Longitude";
	longitude.dim_indicies = { 0 };
	longitude.type = nc_internal::DataType::FLOAT;

	Variable latitude(maingroup_);
	latitude.name = "Latitude";
	latitude.dim_indicies = { 0 };
	latitude.type = nc_internal::DataType::FLOAT;

	Variable executionTime(maingroup_);
	executionTime.name = "ExecutionRuntime";
	executionTime.dim_indicies = { 0 };
	executionTime.type = nc_internal::DataType::FLOAT;

	Variable loadingDataTime(maingroup_);
	loadingDataTime.name = "DriverLoadingRunTime";
	loadingDataTime.dim_indicies = { 0 };
	loadingDataTime.type = nc_internal::DataType::FLOAT;

    Variable stationName(maingroup_);
    stationName.name = "StationName";
    stationName.dim_indicies = { 0 };
    stationName.type = nc_internal::DataType::TEXT;


	maingroup_.variables.push_back(station);
	maingroup_.variables.push_back(time);
	maingroup_.variables.push_back(pfts);
	maingroup_.variables.push_back(stands);
	maingroup_.variables.push_back(longitude);
	maingroup_.variables.push_back(latitude);
	maingroup_.variables.push_back(executionTime);
	maingroup_.variables.push_back(loadingDataTime);
    maingroup_.variables.push_back(stationName);
	writer_->SetMainGroup(maingroup_);

	//push back float variables for lon and lat
	bufsizes_float_.push_back(1);
	n_ncvars_++;
	bufsizes_float_.push_back(1);
	n_ncvars_++;
	
	//push back float variables for execution and loading time
	bufsizes_float_.push_back(1);
	n_ncvars_++;
	bufsizes_float_.push_back(1);
	n_ncvars_++;
}

void BaseWriter::DefineVariables()
{
	for (int s = 0; s < nst; ++s)
	{
		out_guess::PatchCollector& patch_collector = (*stand_patch_collectors_)[s];
		out_guess::PatchPftCollector& patch_pft_collector = (*stand_patch_pft_collectors_)[s];


		Print("OutputVarPatch variables:  \n");

		//Creating NetCDF variables from Patch collector netcdf 4 support
		for (std::shared_ptr<out_guess::OutputVarPatch>& out_var : patch_collector.out_var_refs_)
		{
			switch (out_writer_type_)
			{
			case OutWriterType::Daily:
				if (out_var->IsDaily())
				{
					base_variables_d_.push_back(std::make_shared<NcGuessVar<double>>(NcGuessVar<double>(*out_var, *this, patchgroup_, out_writer_type_, n_years_, true, s)));
					CountNcVars(out_var.get());
				}
				break;
			case OutWriterType::Monthly:
				if (out_var->IsMonthly())
				{
					base_variables_d_.push_back(std::make_shared<NcGuessVar<double>>(NcGuessVar<double>(*out_var, *this, patchgroup_, out_writer_type_, n_years_, true, s)));
					CountNcVars(out_var.get());
				}
				break;
			case OutWriterType::Annualy:
				if (out_var->isYearly())
				{
					base_variables_d_.push_back(std::make_shared<NcGuessVar<double>>(NcGuessVar<double>(*out_var, *this, patchgroup_, out_writer_type_, n_years_, true, s)));
					CountNcVars(out_var.get());
				}
				break;
			case OutWriterType::Invalid:
				break;
			}

			Print(out_var->GetName() + " ");
		}

		Print("\n");
		Print("OutputVarPft variables :  \n");

		//Creating NetCDF variables from Pft collector
		for (std::shared_ptr<out_guess::OutputVarPft>& out_var : patch_pft_collector.out_var_refs_)
		{
			switch (out_writer_type_)
			{
			case OutWriterType::Daily:
				if (out_var->IsDaily())
				{
					base_variables_d_.push_back(std::make_shared<NcGuessVar<double>>(NcGuessVar<double>(*out_var, *this, patchpftgroup_, out_writer_type_, n_years_, true, s)));
					CountNcVars(out_var.get());
				}

				break;
			case OutWriterType::Monthly:
				if (out_var->IsMonthly())
				{
					base_variables_d_.push_back(std::make_shared<NcGuessVar<double>>(NcGuessVar<double>(*out_var, *this, patchpftgroup_, out_writer_type_, n_years_, true, s)));
					CountNcVars(out_var.get());
				}
				break;
			case OutWriterType::Annualy:
				if (out_var->isYearly())
				{
					base_variables_d_.push_back(std::make_shared<NcGuessVar<double>>(NcGuessVar<double>(*out_var, *this, patchpftgroup_, out_writer_type_, n_years_, true, s)));
					CountNcVars(out_var.get());
				}
				break;
			case OutWriterType::Invalid:
				break;
			}

			Print(out_var->GetName() + " ");
		}

	}

	

	EndDef();

	if (writer_type_ == WriterType::Parallel)
	{
		#ifdef HAVE_PNETCDF
		vector<long> bufsizes_int(1);
		bufsizes_int[0] = 1;

		writerPNC_->SetIntBuffer(1, n_proc_gridcells_, bufsizes_int);
		writerPNC_->SetFloatBuffer(n_ncvars_, n_proc_gridcells_, bufsizes_float_);
		Print("Attaching buffer... ");
		writerPNC_->AttachBuffer();
		Print("Done!\n");
		#endif
	}
}

void BaseWriter::SetTimeBeginEnd(int year_begin, int year_end)
{
	switch (opts_.time_range)
	{
	case OutputTimeRange::Scenario:
		year_begin_ = year_begin;
		year_end_ = year_end;
		break;
	case OutputTimeRange::Spinup:
		year_begin_ = year_begin - nyear_spinup;
		year_end_ = year_begin - 1;
		break;
	case OutputTimeRange::Full:
		year_begin_ = year_begin - nyear_spinup;
		year_end_ = year_end;
		break;
	case OutputTimeRange::Custom:
		{

		if (opts_.year_begin < year_begin - nyear_spinup)
		{
			const string error = "year_begin must not be lower than simulation years and spinup. min value allowed: " + std::to_string(year_begin - nyear_spinup) + " value obtained: " + std::to_string(opts_.year_begin);
			throw NcGuessException(error);
		}
		if (opts_.year_end > year_end)
		{
			const string error = "year_end must not be greater than simulation end of simulation. max value allowed: " + std::to_string(year_end) + " value obtained: " + std::to_string(opts_.year_end);
			throw NcGuessException(error);
		}
		if (opts_.year_end - opts_.year_begin < 0)
		{
			const string error = "year_end must be greater equal to year_begin: values obtained: year_end: " + std::to_string(opts_.year_end) + " year_begin: " + std::to_string(opts_.year_begin);
			throw NcGuessException(error);
		}

		year_begin_ = opts_.year_begin;
		year_end_ = opts_.year_end;

		}
		break;
	}

	n_years_= year_end_ - year_begin_ + 1;
}

void BaseWriter::SetStationIndexAndID(size_t index, size_t id)
{
	current_station_id_ = id;
	current_station_index_ = index;
}

void BaseWriter::SetCurrentLonLat(double lon, double lat)
{
	current_lon_ = lon;
	current_lat_ = lat;
}

void BaseWriter::SetCurrentStationName(std::string description)
{
    current_station_name_ = description;
}

void BaseWriter::EndDef() const
{
	Print("EndDef... ");
	writer_->EndDef();
	Print("Done! \n ");
}

void BaseWriter::WriteTime()
{
	//has only to be done once!
	if (!wrote_time_)
	{
		vector<int> timesteps;
		for (size_t step = 0; step < n_timesteps_; step++)
		{
			timesteps.push_back(step);
		}

		//Write the time Variable 
		const vector<size_t> offset = { 0 };
		const vector<size_t> count = { n_timesteps_ };
		const Range range(offset, count);

		Print("Writing time...");
		switch (writer_type_)
		{
		case WriterType::Single:
			#ifndef HAVE_PNETCDF
			writerNC_->WriteData(maingroup_.variables[1], range, timesteps);
			break;
			#endif
		case WriterType::Parallel:
		{
			#ifdef HAVE_PNETCDF
			writerPNC_->WriteDataI(maingroup_.variables[1], range, timesteps);
			#endif
		}
		break;
		}

		Print("Done!  \n");
		wrote_time_ = true;
	}
}

void BaseWriter::WriteStationID()
{
	const vector<size_t> offset = { current_station_index_ };
	const vector<size_t> count = { 1 };
	const Range range(offset, count);
	vector<int> ids(1);
	ids[0] = current_station_id_;

	Print("Write Station id: "+ std::to_string(current_station_id_) + "...");
	switch (writer_type_)
	{
	case WriterType::Single:
	{
		#ifndef HAVE_PNETCDF
		writerNC_->WriteData(maingroup_.variables[0], range, current_station_id_);
		#endif
	}
	break;
	case WriterType::Parallel:
	{
		#ifdef HAVE_PNETCDF
		writerPNC_->WriteDataB(maingroup_.variables[0], range, ids);
		#endif
	}
	break;
	}

	Print("Done! \n");
}

void BaseWriter::WriteStationName()
{
    const vector<size_t> offset = { current_station_index_ };
    const vector<size_t> count = { 1 };
    const Range range(offset, count);
    vector<string> names(1);
    names[0] = current_station_name_;

    Print("Write Station Name: "+ current_station_name_ + "...");
    switch (writer_type_)
    {
        case WriterType::Single:
        {
#ifndef HAVE_PNETCDF
            //static_cast<float>
            writerNC_->WriteData(maingroup_.variables[8], range, names);
#endif
        }
            break;
        case WriterType::Parallel:
        {
#ifdef HAVE_PNETCDF
            writerPNC_->WriteDataB(maingroup_.variables[8], range, names);
#endif
        }
            break;
    }

    Print("Done! \n");
}

void BaseWriter::WritePfts()
{
	//has only to be done once!
	if (!wrote_pfts_)
	{
		vector<string> pfts;
		for (size_t i = 0; i < npft; i++)
		{
			pfts.push_back(pftlist[i].name.cstr());
		}
		pfts.push_back("Total");

		const vector<size_t> offset = { 0 };
		vector<size_t> count = { static_cast<size_t>(npft + 1) };
		const Range range(offset, count);

		Print("Writing Pfts...");
		switch (writer_type_)
		{
		case WriterType::Single:
		{
			#ifndef HAVE_PNETCDF
			writerNC_->WriteData(maingroup_.variables[2], range, pfts);
			#endif
		}
		break;
		case WriterType::Parallel:
		{
			#ifdef HAVE_PNETCDF
			//PNetCDF does not support vector of strings...
			//we put pfts as attributes here
			//char delimiter = ',' ;
			//string pft_string;
			//for (const std::string& pft : pfts)
			//{
			//	pft_string += pft + delimiter;
			//}
			//pft_string.pop_back();
			//pft_string = "as";
			//writerPNC_->WriteDataI(maingroup_.variables[2], range, pft_string);
			#endif
		}
		break;
		}

		Print("Done! \n");
		wrote_pfts_ = true;
	}

}

void BaseWriter::WriteStands()
{
	//has only to be done once!
	if (!wrote_pfts_)
	{
		vector<string> sts;
		for (size_t i = 0; i < nst; i++)
		{
			if (i == 0)
			{
				sts.push_back("NATURAL");
			}
			else
			{
				sts.push_back(stlist[i - 1].name.cstr());
			}
			
		}

		const vector<size_t> offset = { 0 };
		vector<size_t> count = { static_cast<size_t>(nst) };
		const Range range(offset, count);

		Print("Writing Stands...");
		switch (writer_type_)
		{
		case WriterType::Single:
		{
#ifndef HAVE_PNETCDF
			writerNC_->WriteData(maingroup_.variables[3], range, sts);
#endif
		}
		break;
		case WriterType::Parallel:
		{
#ifdef HAVE_PNETCDF
			//PNetCDF does not support vector of strings...
			//we put pfts as attributes here
			//char delimiter = ',' ;
			//string st_string;
			//for (const std::string& st : sts)
			//{
			//	st_string += st + delimiter;
			//}
			//pft_string.pop_back();
			//pft_string = "as";
			//writerPNC_->WriteDataI(maingroup_.variables[3], range, st_string);
#endif
		}
		break;
		}

		Print("Done! \n");
		//wrote_pfts_ = true;
	}
}

void BaseWriter::WriteLonLat()
{
	const vector<size_t> offset = { current_station_index_ };
	const vector<size_t> count = { 1 };
	const Range range(offset, count);

	vector<float> lon(1);
	lon[0] = current_lon_;
	vector<float> lat(1);
	lat[0] = current_lat_;

	Print("Writing Lon: " + std::to_string(lon[0]) +  " Lat: " + std::to_string(lat[0]));

	switch (writer_type_)
	{
	case WriterType::Single:
	{
		#ifndef HAVE_PNETCDF
		writerNC_->WriteData(maingroup_.variables[4], range, static_cast<float>(current_lon_));
		writerNC_->WriteData(maingroup_.variables[5], range, static_cast<float>(current_lat_));
		#endif
	}
	break;
	case WriterType::Parallel:
	{
		#ifdef HAVE_PNETCDF
		writerPNC_->WriteDataB(maingroup_.variables[4], range, lon);
		writerPNC_->WriteDataB(maingroup_.variables[5], range, lat);
		#endif
	}
	break;
	}
	Print("Done! \n");
}

void BaseWriter::WriteExecutionTime(double elapsed_ms, double input_ms)
{
	const vector<size_t> offset = { current_station_index_ };
	const vector<size_t> count = { 1 };
	const Range range(offset, count);
	
	vector<float> executionTime(1);
	executionTime[0] = static_cast<float>(elapsed_ms);
	vector<float> loadingTime(1);
	loadingTime[0] = static_cast<float>(input_ms);

	Print("Writing EX-TIME: " + std::to_string(executionTime[0]) + " LOAD-TIME: " + std::to_string(loadingTime[0]));

	switch (writer_type_)
	{
	case WriterType::Single:
	{
#ifndef HAVE_PNETCDF
		writerNC_->WriteData(maingroup_.variables[6], range, static_cast<float>(elapsed_ms));
		writerNC_->WriteData(maingroup_.variables[7], range, static_cast<float>(input_ms));
#endif
	}
	break;
	case WriterType::Parallel:
	{
#ifdef HAVE_PNETCDF
		writerPNC_->WriteDataB(maingroup_.variables[6], range, executionTime);
		writerPNC_->WriteDataB(maingroup_.variables[7], range, loadingTime);
#endif
	}
	break;
	}
	Print("Done! \n");
}

void BaseWriter::WriteAllVariables()
{
	Print("Writing Variables...");

	for (std::shared_ptr<NcGuessVar<double>>& variable : base_variables_d_)
	{
		size_t y_size_re = time_res_;
		size_t z_size_re = 0;

		const size_t std_id = static_cast<size_t>(variable->GetStandID());

		vector<size_t> offsets;
		vector<size_t> counts;

		if (variable->IsPatchVar())
		{
			switch (variable->PatchDetailLevel())
			{
			case out_guess::PatchDetailLevel::Each_Patch:
			{
				offsets = { current_station_index_, 0, 0 , std_id};
				counts = { 1 , n_timesteps_ , static_cast<size_t>(npatch) , 1 };
			}
			break;
			case out_guess::PatchDetailLevel::Mean_over_all:
			{
				offsets = { current_station_index_, 0 , std_id};
				counts = { 1 , n_timesteps_  , 1 };
			}
			break;
			case out_guess::PatchDetailLevel::Invalid:
				string error = "This should not happen, did you change something of the PatchDetailLevel enum?!";
				throw(error);
			break;
			}
		}

		else if (variable->IsPftVar())
		{
			switch (variable->PftDetailLevel())
			{
			case out_guess::PftDetailLevel::Total:
				offsets = { current_station_index_, 0, std_id };
				counts = { 1 , n_timesteps_, 1 };
			break;
			case out_guess::PftDetailLevel::Pfts_Total:
			{
				offsets = { current_station_index_ , 0 ,0, std_id };
				counts = { 1, n_timesteps_, static_cast<size_t>(npft + 1), 1 };
			}
			break;

			case out_guess::PftDetailLevel::Invalid:
				string error = "This should not happen, did you change something of the PftDetailLevel enum?!";
				throw(error);
				break;
			}


			
		}

		const vector<double> data_f = variable->GetData2D()->Transpose().GetAllValues();
		const Range range(offsets, counts);

		Print(" " + variable->GetName());

		switch (writer_type_)
		{
		case WriterType::Single:
		{
			#ifndef HAVE_PNETCDF
			Variable nc_var = writerNC_->GetVariable(variable->GetGroupName(), variable->GetName());
			writerNC_->WriteData(nc_var, range, data_f);
			#endif
		}
		break;
		case WriterType::Parallel:
		{
			#ifdef HAVE_PNETCDF
			Variable nc_var = writerPNC_->GetVariable(variable->GetGroupName(), variable->GetName());
			writerPNC_->WriteDataB(nc_var, range, data_f);
			#endif 
		}
		break;
		}
		//The following does NOT work as the NcVariable defined in the ouput does not have the correct ID
		//Variable nc_var = variable->GetNcVar();	
	}

	Print("Done! \n");
}

void BaseWriter::AccuireData()
{
	if (date.get_calendar_year() >= year_begin_ && date.get_calendar_year() <= year_end_)
	{
		for (std::shared_ptr<NcGuessVar<double>>& nc_var : base_variables_d_)
		{
			nc_var->AddValues();
		}
	}
}

void BaseWriter::ClearData()
{
	for (std::shared_ptr<NcGuessVar<double>>& nc_var : base_variables_d_)
	{
		nc_var->Clear();
	}
}

void BaseWriter::Close() const
{
	writer_->Close();
}

void BaseWriter::Finalize()
{
	if (writer_type_ == WriterType::Parallel)
	{
		std::string s = std::to_string(Parallel::get().get_rank()) + ": Detaching buffer...";
		Print(s);
		#ifdef HAVE_PNETCDF
		writerPNC_->FullBufferedEnd();
		#endif
		Print("Done! \n");
	}

}

size_t BaseWriter::NGridcells() const
{
	return n_proc_gridcells_;
}

size_t BaseWriter::NTimeSteps() const
{
	return n_timesteps_;
}

size_t BaseWriter::CurrentStationID() const
{
	return current_station_id_;
}

size_t BaseWriter::CurrentStationIndex() const
{
	return current_station_index_;
}

int BaseWriter::YearBegin() const
{
	return year_begin_;
}

int BaseWriter::YearEnd() const
{
	return year_end_;
}

void BaseWriter::WriteAllVariablesSingle(double* data, int& offset)
{
	Print("Writing Variables...");

	int raw_data_offset = offset;

	for (std::shared_ptr<NcGuessVar<double>>& variable : base_variables_d_)
	{
		size_t y_size_re = time_res_;
		size_t z_size_re = 0;

		const size_t std_id = static_cast<size_t>(variable->GetStandID());

		vector<size_t> offsets;
		vector<size_t> counts;

		if (variable->IsPatchVar())
		{
			switch (variable->PatchDetailLevel())
			{
			case out_guess::PatchDetailLevel::Each_Patch:
			{
				offsets = { current_station_index_, 0, 0 , std_id };
				counts = { 1 , n_timesteps_ , static_cast<size_t>(npatch) , 1 };
			}
			break;
			case out_guess::PatchDetailLevel::Mean_over_all:
			{
				offsets = { current_station_index_, 0, std_id };
				counts = { 1 , n_timesteps_ , 1 };
			}
			break;
			case out_guess::PatchDetailLevel::Invalid:
				string error = "This should not happen, did you change something of the PatchDetailLevel enum?!";
				throw(error);
				break;
			}
		}

		else if (variable->IsPftVar())
		{
			switch (variable->PftDetailLevel())
			{
			case out_guess::PftDetailLevel::Total:
				offsets = { current_station_index_, 0, std_id };
				counts = { 1 , n_timesteps_ , 1 };
				break;
			case out_guess::PftDetailLevel::Pfts_Total:
			{
				offsets = { current_station_index_ , 0 , 0 , std_id };
				counts = { 1, n_timesteps_, static_cast<size_t>(npft + 1), 1 };
			}
			break;

			case out_guess::PftDetailLevel::Invalid:
				string error = "This should not happen, did you change something of the PftDetailLevel enum?!";
				throw(error);
				break;
			}
		}

		int count_raw_data = 1;
		for (size_t count : counts)
			count_raw_data *= count;
		
		vector<double> data_f(count_raw_data);

		for (int i = 0; i < count_raw_data; ++i)
		{
			data_f[i] = data[raw_data_offset + i];
		}

		raw_data_offset += count_raw_data;

		const Range range(offsets, counts);
		


		//auto start = std::chrono::high_resolution_clock::now();
		#ifndef HAVE_PNETCDF
		Variable nc_var = writerNC_->GetVariable(variable->GetGroupName(), variable->GetName());
		writerNC_->WriteData(nc_var, range, data_f);
		#endif
		//auto end = std::chrono::high_resolution_clock::now();

		//auto writing_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		//std::string s = std::to_string((int)this->out_writer_type_) + "Timessss " + std::to_string(writing_ms) + " ms\n";
		//dprintf(s.c_str());

	}
	
	offset = raw_data_offset;

	Print("Done! \n");

}


void BaseWriter::CountNcVars(const out_guess::OutputVarPatch* out_var)
{
	switch (out_var->GetPatchDetailLevel())
	{
	case out_guess::PatchDetailLevel::Each_Patch:
		n_ncvars_++;
		bufsizes_float_.push_back(npatch * n_timesteps_);
		break;
	case out_guess::PatchDetailLevel::Mean_over_all:
		n_ncvars_++;
		bufsizes_float_.push_back(1 * n_timesteps_);
		break;
	case out_guess::PatchDetailLevel::Invalid:
		break;
	default:;
	}
}

void BaseWriter::CountNcVars(const out_guess::OutputVarPft* out_var)
{
	switch (out_var->GetPftDetailLevel())
	{
	case out_guess::PftDetailLevel::Total:
		n_ncvars_++;
		bufsizes_float_.push_back(1 * n_timesteps_);
		break;
	case out_guess::PftDetailLevel::Pfts_Total:
		n_ncvars_++;
		bufsizes_float_.push_back((npft + 1)*n_timesteps_);
		break;
	case out_guess::PftDetailLevel::Invalid:
		break;
	default:;
	}
}

void BaseWriter::Print(const string& message) const
{
	opts_.Print(message);
}

void BaseWriter::AddPftAttributes(Variable& var) const
{

	for (size_t i = 0; i < npft; i++)
	{
		Attribute attr;
		attr.name = "Pft-" + std::to_string(i + 1);
		attr.value = pftlist[i].name.cstr();
		var.attributes.push_back(attr);
	}

	Attribute tot;
	tot.name = "Pft-" + std::to_string(npft + 1);
	tot.value= "Total";
	var.attributes.push_back(tot);
}


void BaseWriter::GatherData()
{
	
	//data_.push_back(current_lon_);
	//data_.push_back(current_lat_);

	for (std::shared_ptr<NcGuessVar<double>>& variable : base_variables_d_)
	{

		const vector<double> data_f = variable->GetData2D()->Transpose().GetAllValues();

		for (int i = 0; i < data_f.size(); ++i)
		{
			data_.push_back(data_f[i]);
			//dprintf(std::to_string(data_f[i]).c_str());
			//dprintf(" ");
		}
			
	}


	// Send all the data to the root process, he will take care and write it down.
	//mpi_gather_module_->SendData(data_to_send);
}

//void BaseWriter::ConnectToMPIModule(nc_guess_interal::MPI_Gridlist_Data_Send* sender)
//{
//
//	mpi_gather_module_ = sender;
//}




