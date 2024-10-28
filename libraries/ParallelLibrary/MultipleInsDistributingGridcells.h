#pragma once
#include "GuessSystemBuilder.h"

class MultipleInsDistributingGridcells: public GuessSystemBuilder
{
public:
	MultipleInsDistributingGridcells(const CommandLineArguments& args);
	~MultipleInsDistributingGridcells();
	void run() override;
};

