#pragma once
#include "BaseWriter.h"

class SingleWriterAnnualy : public BaseWriter
	{
	public:
		SingleWriterAnnualy(SmartOutputBuilderOptions& opts, const std::string ouput_directory);
		~SingleWriterAnnualy();

		void Init() override;
		void SetTimeSteps() override;
	};

