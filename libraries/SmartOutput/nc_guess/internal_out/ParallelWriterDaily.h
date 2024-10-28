#pragma once
#include "BaseWriter.h"
#ifdef HAVE_PNETCDF

namespace nc_guess_interal
{

	class ParallelWriterDaily : public BaseWriter
	{
	public:
		ParallelWriterDaily(SmartOutputBuilderOptions& opts, const std::string ouput_directory);
		~ParallelWriterDaily();
		void Init() override;
		void SetTimeSteps() override;
	};

}

#endif

