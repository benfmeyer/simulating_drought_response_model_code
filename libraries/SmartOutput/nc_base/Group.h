#pragma once
#include <string>
#include <vector>
#include "Variable.h"
using std::string;
using std::vector;

namespace nc_base
{
	struct Group
	{
		Group(string name, int id)
		{
			this->name = name;
			this->group_ID = id;
		}

		Group(string name) : group_ID(-1)
		{
			this->name = name;
		}

		Group() : group_ID(-1)
		{

		}

		int group_ID;
		string name;
		size_t n_dims = -1;
		vector<Variable> variables;
		vector<Attribute> global_attributes;
	};
}
