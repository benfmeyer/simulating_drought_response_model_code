#pragma once
#include "GuessSystemBuilder.h"

class MultipleInsDistributingInsfiles : public GuessSystemBuilder
{
public:
	MultipleInsDistributingInsfiles(const CommandLineArguments& args);
	~MultipleInsDistributingInsfiles();
	void run() override;
};

