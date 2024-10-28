#include "Reader.h"


Reader::Reader(string filename):InternalReader(filename), BaseFileNC(filename)
{
	Init(Mode::ReadOnly);
}


Reader::~Reader()
{
	delete main_group_;
	for (size_t i = 0; i < groups_.size(); i++)
	{
		delete groups_[i];
	}
	for (size_t i = 0; i < dimensions_.size(); i++)
	{
		delete dimensions_[i];
	}
}
