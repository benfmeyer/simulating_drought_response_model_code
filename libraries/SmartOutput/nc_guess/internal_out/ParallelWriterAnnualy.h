#pragma once
#include "BaseWriter.h"
#ifdef HAVE_PNETCDF

namespace nc_guess_interal
{
	class ParallelWriterAnnualy : public BaseWriter
	{
	public:
		ParallelWriterAnnualy(SmartOutputBuilderOptions& opts, const std::string ouput_directory);
		~ParallelWriterAnnualy();

		void Init() override;
		void SetTimeSteps() override;
	};
}



#endif