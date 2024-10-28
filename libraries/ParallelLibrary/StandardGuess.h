#pragma once
#include "GuessSystemBuilder.h"
class StandardGuess: public GuessSystemBuilder
{
public:
	StandardGuess(const CommandLineArguments& args);
	~StandardGuess();
	void run() override;
};

