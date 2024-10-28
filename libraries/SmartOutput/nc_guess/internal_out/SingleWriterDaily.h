#pragma once
#include "BaseWriter.h"

	class SingleWriterDaily :public BaseWriter
	{
	public:
		SingleWriterDaily(SmartOutputBuilderOptions& opts, const std::string ouput_directory);
		~SingleWriterDaily();
		void Init() override;
		void SetTimeSteps() override;
	};