#pragma once
#include "BaseWriter.h"

	class SingleWriterMonthly : public BaseWriter
	{
	public:
		SingleWriterMonthly(SmartOutputBuilderOptions& opts,  const std::string ouput_directory);
		~SingleWriterMonthly();
		void Init() override;
		void SetTimeSteps() override;
	};


