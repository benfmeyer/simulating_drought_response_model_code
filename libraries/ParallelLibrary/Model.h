#pragma once
#include "commandlinearguments.h"
#include "gutil.h"
#include <memory>
#include "GuessSystemBuilder.h"

class Model
{
public:
	Model();
	~Model();

	void init(int argc, char** argv);

	void run();

private:
	std::unique_ptr<CommandLineArguments> args;
	std::unique_ptr<GuessSystemBuilder> model;
};



