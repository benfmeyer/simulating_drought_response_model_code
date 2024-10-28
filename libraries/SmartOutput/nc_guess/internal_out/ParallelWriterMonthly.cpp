#ifdef HAVE_PNETCDF
#include "ParallelWriterMonthly.h"
#include "parallel.h"
namespace nc_guess_interal
{
	ParallelWriterMonthly::ParallelWriterMonthly(SmartOutputBuilderOptions& opts, const std::string ouput_directory) : BaseWriter(opts, ouput_directory)
	{
		writer_type_ = WriterType::Parallel;
		out_writer_type_ = OutWriterType::Monthly;
		time_unit_ = "months";
		time_res_ = 12;
	}


	ParallelWriterMonthly::~ParallelWriterMonthly()
	{
	}

	void ParallelWriterMonthly::Init()
	{
		filename_ = "../MonthlyOut.nc";
		writer_ = std::make_unique<WriterPNC>(filename_, Mode::CreateParallel, Parallel::get().get_communicator());

		BaseWriter::Init();
	}

	void ParallelWriterMonthly::SetTimeSteps()
	{
		n_timesteps_ = n_years_ * time_res_;
	}
}
#endif
