#pragma once
#include "BaseWriter.h"
#ifdef HAVE_PNETCDF

namespace nc_guess_interal
{
	class ParallelWriterMonthly : public BaseWriter
	{
	public:
		ParallelWriterMonthly(SmartOutputBuilderOptions& opts, const std::string ouput_directory);
		~ParallelWriterMonthly();

		void Init() override;
		void SetTimeSteps() override;

	};

}

#endif

