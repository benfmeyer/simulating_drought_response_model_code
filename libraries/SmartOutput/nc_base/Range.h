#pragma once
#include <vector>

using std::vector;
namespace  nc_base
{
	struct Range
	{
		vector<size_t> offsets;
		vector<size_t> counts;

		Range(const vector<size_t> offsets, const vector<size_t> counts)
		{
			this->offsets = offsets;
			this->counts = counts;			
		}
	};
}