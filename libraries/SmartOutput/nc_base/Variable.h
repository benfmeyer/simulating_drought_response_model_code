#pragma once
#include <vector>
#include <string>
#include "Attribute.h"
#include "Types.h"

using std::string;
using std::vector;

namespace nc_base
{
	struct Group;

	struct Variable
	{
		string name;
		nc_internal::DataType type;
		Group& group;

		int var_ID;
		nc_internal::DeflationLevel def_level;
		vector<int> dim_indicies;

		nc_internal::StorageType storage;
		vector<Attribute> attributes;
		vector<size_t> chunk_sizes;

		Variable();
		Variable(Group& group);
		Variable(const string& name, nc_internal::DataType type, Group& group);
	};


	inline Variable::Variable(Group& group) :group(group), def_level(nc_internal::DeflationLevel::None)
	{
	}

	inline Variable::Variable(const string& name, nc_internal::DataType type, Group& group) : name(name),
		type(type),
		group(group),
		def_level(nc_internal::DeflationLevel::None)
	{

	}
}

