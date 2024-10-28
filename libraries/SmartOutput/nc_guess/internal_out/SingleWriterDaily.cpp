#include "SingleWriterDaily.h"

	SingleWriterDaily::SingleWriterDaily(SmartOutputBuilderOptions& opts, const std::string ouput_directory) : BaseWriter(opts, ouput_directory)
	{
		writer_type_ = WriterType::Single;
		out_writer_type_ = OutWriterType::Daily;
		time_unit_ = "days";
		time_res_ = 365;
	}


	SingleWriterDaily::~SingleWriterDaily()
	{
	}

	void SingleWriterDaily::Init()
	{		
		filename_ = "DailyOut.nc";
		#ifndef HAVE_PNETCDF 
		writer_ = std::make_unique<WriterNC>(output_directory_ + filename_, Mode::Create, data_format_);
		#endif
		BaseWriter::Init();
	}

	void SingleWriterDaily::SetTimeSteps()
	{
		n_timesteps_ = n_years_ * time_res_;
	}
