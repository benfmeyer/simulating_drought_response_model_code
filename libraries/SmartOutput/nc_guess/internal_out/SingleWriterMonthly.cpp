#include "SingleWriterMonthly.h"



SingleWriterMonthly::SingleWriterMonthly(SmartOutputBuilderOptions& opts, const std::string ouput_directory) : BaseWriter(opts, ouput_directory)
	{
		writer_type_ = WriterType::Single;
		out_writer_type_ = OutWriterType::Monthly;
		time_unit_ = "months";
		time_res_ = 12;		
	}


	SingleWriterMonthly::~SingleWriterMonthly()
	{
	}

	void SingleWriterMonthly::Init()
	{
		filename_ = "MonthlyOut.nc";
		#ifndef HAVE_PNETCDF 
		writer_ = std::make_unique<WriterNC>(output_directory_ + filename_, Mode::Create, data_format_);
		#endif
		BaseWriter::Init();
	}

	void SingleWriterMonthly::SetTimeSteps()
	{
		n_timesteps_ = n_years_ * time_res_;
	}
