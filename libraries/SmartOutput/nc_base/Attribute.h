#pragma once
#include <string>
#include "Types.h"
using std::string;

namespace nc_base
{
	struct Attribute
	{
		string name;
		int id;
		string value;
		nc_internal::DataType type;

		Attribute(string name, string value, nc_internal::DataType type, int id)
		{
			this->name = name;
			this->value = value;
			this->id = id;
			this->type = type;
		}
		Attribute(string name, string value, nc_internal::DataType type)
		{
			this->name = name;
			this->value = value;
			this->type = type;
		}

		Attribute(string name, string value)
		{
			this->name = name;
			this->value = value;
			this->type = nc_internal::DataType::TEXT;
		}

		Attribute() : id(-1), type(nc_internal::DataType::INT)
		{
		}
	};
}
