#ifdef HAVE_PNETCDF
#include "ParallelWriterDaily.h"
#include "parallel.h"

namespace nc_guess_interal
{
	ParallelWriterDaily::ParallelWriterDaily(SmartOutputBuilderOptions& opts, const std::string ouput_directory) : BaseWriter(opts, ouput_directory)
	{
		writer_type_ = WriterType::Parallel;
		out_writer_type_ = OutWriterType::Daily;
		time_unit_ = "days";
		time_res_ = 365;
	}


	ParallelWriterDaily::~ParallelWriterDaily()
	{

	}

	void ParallelWriterDaily::Init()
	{
		filename_ = "../DailyOut.nc";
		writer_ = std::make_unique<WriterPNC>(filename_, Mode::CreateParallel, Parallel::get().get_communicator());

		BaseWriter::Init();
	}

	void ParallelWriterDaily::SetTimeSteps()
	{
		n_timesteps_ = n_years_ * time_res_;
	}
}
#endif
