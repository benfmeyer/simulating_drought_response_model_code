#pragma once
#include <string>
using std::string;

namespace nc_base
{
	struct Dimension
	{
		string name;
		size_t count;
		int index;
	};
}
