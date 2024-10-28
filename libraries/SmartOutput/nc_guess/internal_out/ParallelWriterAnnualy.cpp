#ifdef HAVE_PNETCDF

#include "ParallelWriterAnnualy.h"
#include "parallel.h"

namespace nc_guess_interal
{
	ParallelWriterAnnualy::ParallelWriterAnnualy(SmartOutputBuilderOptions& opts, const std::string ouput_directory) : BaseWriter(opts, ouput_directory)
	{
		writer_type_ = WriterType::Parallel;
		out_writer_type_ = OutWriterType::Annualy;
		time_unit_ = "years";
		time_res_ = 1;
	}


	ParallelWriterAnnualy::~ParallelWriterAnnualy()
	{

	}

	void ParallelWriterAnnualy::Init()
	{
		filename_ = "../AnnuallyOut.nc";
		writer_ = std::make_unique<WriterPNC>(filename_, Mode::CreateParallel, Parallel::get().get_communicator());

		BaseWriter::Init();
	}

	void ParallelWriterAnnualy::SetTimeSteps()
	{
		n_timesteps_ = n_years_ * time_res_;
	}
}
#endif