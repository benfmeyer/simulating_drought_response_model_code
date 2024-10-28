#include "SingleWriterAnnualy.h"

	SingleWriterAnnualy::SingleWriterAnnualy(SmartOutputBuilderOptions& opts, const std::string ouput_directory) : BaseWriter(opts, ouput_directory)
	{
		writer_type_ = WriterType::Single;
		out_writer_type_ = OutWriterType::Annualy;
		time_unit_ = "years";
		time_res_ = 1;
	}


	SingleWriterAnnualy::~SingleWriterAnnualy()
	{
	}

	void SingleWriterAnnualy::Init()
	{
		filename_ = "AnnuallyOut.nc";
		#ifndef HAVE_PNETCDF 
		writer_ = std::make_unique<WriterNC>(output_directory_ + filename_, Mode::Create, data_format_);
		#endif
		BaseWriter::Init();
	}

	void SingleWriterAnnualy::SetTimeSteps()
	{
		n_timesteps_ = n_years_ * time_res_;
	}
