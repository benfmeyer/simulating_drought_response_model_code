#include "InternalReader.h"
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

nc_internal::InternalReader::InternalReader(string filename): BaseFileNC(filename)
{
}

nc_internal::InternalReader::InternalReader():BaseFileNC()
{
}

void nc_internal::InternalReader::Inquery()
{
	InqGroups();
	InqBaseInfo(*main_group_);

	for (size_t i = 0; i < groups_.size(); i++)
	{
		InqBaseInfo(*groups_[i]);
	}

	for (size_t i = 0; i < groups_.size(); i++)
	{
		InqBaseInfo(*groups_[i]);
	}
}

void nc_internal::InternalReader::PrintInfos()
{
	cout << "Printing informations about file:" << endl;
	cout << filename_ << endl;
	cout << "Found the following global informations: " << endl;
	cout << "Number of Groups: " << groups_.size() << endl;
	cout << "Number of Dimensions: " << dimensions_.size() << endl;
	cout << endl;

	if (dimensions_.size() > 0)
	{
		cout << "Dimensions:" << endl;
		for (auto& dim : dimensions_)
		{
			cout << "Name: " << dim->name << ": " << "Size: " << dim->count << " Index: " << dim->index << endl;
		}
		cout << "-----------------------------------------------------------" << endl;
	}
	cout << endl;

	for (Group* group : groups_)
	{
		cout << "Name: " << group->name << endl;
		cout << "Number of Variables" << group->variables.size() << endl;
		cout << "Number of Global Attributes" << group->global_attributes.size() << endl;
		cout << "-----------------------------------------------------------" << endl;
		if (group->global_attributes.size() > 0)
		{
			cout << "Global Atttributes:" << endl;
			for (Attribute attr : group->global_attributes)
			{
				cout << "A-Name: " << attr.name << ", Type, " << (char*)attr.type << ", Value:  " << attr.value << endl;
			}
			cout << "-----------------------------------------------------------" << endl;
		}

		if (group->variables.size() > 0)
		{
			cout << "Variables:" << endl;
			//for (Variable& var : group.variables)
			//{
			//	cout << "Name: " << var.name << endl;
			//	cout << "Dimension Numbers: " << var.dim_indicies.size() << endl;
			//	cout << "Corresponding Dimensions: ";
			//	for (int index : var.dim_indicies)
			//	{
			//		cout << index << " ";
			//	}
			//	cout << endl;

			//	for (Attribute attr : var.attributes)
			//	{
			//		cout << "A-Name: " << attr.name << ", Type, " << attr.type << ", Value:  " << attr.value << endl;
			//	}
			//}
			cout << "-----------------------------------------------------------" << endl;
		}
	}
}

Group& nc_internal::InternalReader::GetMainGroup()
{
	return *main_group_;
}

vector<Group> nc_internal::InternalReader::GetAllGroups()
{
	vector<Group> groups;
	for (size_t i = 0; i < groups_.size(); i++)
	{
		//groups.push_back(*groups_[i].get());
	}
	return groups;
}

Group& nc_internal::InternalReader::GetGroup(string name)
{
	if (main_group_->name == name)
	{
		return *main_group_;
	}

	for (size_t i = 0; i < groups_.size(); i++)
	{
		if (groups_[i]->name == name)
		{
			return *groups_[i];
		}		
	}
	cout << "Could not find group with name: " << name << endl;
	throw;
}

Variable& nc_internal::InternalReader::GetVariable(string grpname, string varname)
{
	Group& group = GetGroup(grpname);

	std::transform(varname.begin(), varname.end(), varname.begin(), ::tolower);

	for (size_t i = 0; i < group.variables.size(); i++)
	{
		if (group.variables[i].name == varname)
		{
			return group.variables[i];
		}
	}
	cout << "Could not find variable with name: " << varname << endl;
	throw;
}

void nc_internal::InternalReader::InqGroups()
{
	//Get the main group first

	int ngroups = -1;
	status_ = nc_inq_grps(main_ncid_, &ngroups, nullptr);
	HandleError(status_);

	// We have a netcdf-4 file and groups
	if (ngroups > 0)
	{
		//Make sure the group size is zero
		groups_.clear();

		vector<int> nc_group_IDs;
		nc_group_IDs.resize(ngroups);

		status_ = nc_inq_grps(main_ncid_, nullptr, &nc_group_IDs.front());
		HandleError(status_);		
	
		for (int i = 0; i < ngroups; ++i)
		{
			size_t lenp;
			nc_inq_grpname_len(nc_group_IDs[i], &lenp);

			vector<char> name_chr;
			name_chr.resize(lenp);

			nc_inq_grpname(nc_group_IDs[i], &name_chr.front());

			Group group;
			group.name = CleanString(string(name_chr.begin(), name_chr.end()));
			group.group_ID = nc_group_IDs[i];

			if (i == 0)
			{
				main_group_ = new Group(group.name, group.group_ID);
			}
			else
			{
				groups_.push_back(new Group(group.name, group.group_ID));
			}
		}		
	}

	else
	{
		main_group_ = new Group();
		main_group_->name = "Main";
		main_group_->group_ID = main_ncid_;
	}
}

void nc_internal::InternalReader::InqBaseInfo(Group& group)
{
	int n_dims = -1;
	int n_vars = -1;
	int n_atts = -1;

	status_ = nc_inq(group.group_ID, &n_dims, &n_vars, &n_atts, nullptr);
	HandleError(status_);

	//convert Int to size_t
	group.n_dims = n_dims;
	group.variables.clear();
	group.global_attributes.resize(n_atts);


	//Get all the global attributes;
	InqGroupAttributes(group);
	//Get all dimension;
	InqDimensions(group);
	//Get all variables;
	InqVariables(group, n_vars);
}

void nc_internal::InternalReader::InqGroupAttributes(Group& group)
{
	//Get global attributes;
	//This is done via a temporary Variable with NC_GLOBAL-VAR-ID
	Variable globalVar(group);
	globalVar.var_ID = NC_GLOBAL;
	globalVar.name = "Global Variable";
	InqAttributes(globalVar);

	group.global_attributes = globalVar.attributes;
}

void nc_internal::InternalReader::InqDimensions(Group& group)
{	
	//Make sure dimensions do not contain anything
	dimensions_.clear();

	for (size_t i = 0; i < group.n_dims; ++i)
	{
		char nameOfDim[NC_MAX_CHAR];
		size_t sizeOfDim;
		//Get the informations about the dimension.
		status_ = nc_inq_dim(group.group_ID, i, nameOfDim, &sizeOfDim);
		HandleError(status_);

		Dimension dim;
		dim.name = nameOfDim;
		dim.count = sizeOfDim;
		dim.index = i;
		//ToDo double-Check
		dimensions_.push_back(new Dimension(dim));
	}
}

void nc_internal::InternalReader::InqVariables(Group& group, size_t n_vars)
{
	for (size_t var_id = 0; var_id < n_vars; ++var_id)
	{
		char nameOfVar[NC_MAX_CHAR];
		nc_type type;
		int numberOfDims;

		//For now we dont care about global attributes.
		int dimidsp[NC_MAX_CHAR];
		int attsp[NC_MAX_CHAR];

		//Try to get variable
		status_ = nc_inq_var(group.group_ID, var_id, nameOfVar, &type, &numberOfDims, dimidsp, attsp);
		HandleError(status_);

		//If everything went we'll create a new variable
		Variable variable(group);
		variable.var_ID = var_id;
		variable.name = CleanString(nameOfVar);


		//Todo i  mprove
		//switch (type)
		//{
		//	default: ;
		//}
		variable.type = DataType::FLOAT;

		//Get the attributes of the variable
		InqAttributes(variable);

		//Get the dimensionIndicies of the variable
		InqDimensionIndicies(variable, numberOfDims);

		//Get the chunck sizes
		InqChunckSizes(variable);

		//If everthing worked well, push the variable to the variable list.
		group.variables.push_back(variable);
	}
}

void nc_internal::InternalReader::InqAttributes(Variable& var)
{
	int numberOfAttributesPerVariable = 0;

	// Get the number of Attributes per variable
	status_ = nc_inq_varnatts(var.group.group_ID, var.var_ID, &numberOfAttributesPerVariable);
	HandleError(status_);

	for (int att = 0; att < numberOfAttributesPerVariable; ++att)
	{
		//Get the attribute name.
		vector<char> name(NC_MAX_CHAR, 0);
		status_ = nc_inq_attname(var.group.group_ID, var.var_ID, att, &name.front());
		HandleError(status_);
		//Erasing the zero elements of the name.
		name.erase(std::remove_if(name.begin(), name.end(), [](const char& c) { return c == '\0'; }), name.end());

		//Obtain type of var
		nc_type type;
		status_ = nc_inq_atttype(var.group.group_ID, var.var_ID, &name.front(), &type);
		HandleError(status_);

		//Create an temporary attribute
		Attribute attribute;
		attribute.name = string(name.begin(), name.end());
		attribute.id = att;


		//Todo i  mprove
		//switch (type)
		//{

		//default:;
		//}
		attribute.type = DataType::FLOAT;

		//Get the attribute values
		attribute.value = GetAttributeValue(var, attribute, name);

		//If everything went fine add attribute
		var.attributes.push_back(attribute);
	}
}

void nc_internal::InternalReader::InqDimensionIndicies(Variable& var, size_t ndims)
{
	var.dim_indicies.resize(ndims);
	for (int dim = 0; dim < ndims; ++dim)
	{
		status_ = nc_inq_vardimid(var.group.group_ID, var.var_ID, &var.dim_indicies.front());
		HandleError(status_);
	}
}

void nc_internal::InternalReader::InqChunckSizes(Variable& var)
{
	var.chunk_sizes.resize(var.dim_indicies.size());
	int storage;
	status_ = nc_inq_var_chunking(var.group.group_ID, var.var_ID, &storage, &var.chunk_sizes.front());
	HandleError(status_);
	var.storage = static_cast<StorageType>(storage);
}

string nc_internal::InternalReader::GetAttributeValue(const Variable& var, Attribute& attr, const vector<char>& internalName)
{
	switch (attr.type)
	{
	case(DataType::TEXT) :
	{
		//Get the length of the attributes value
		size_t attlen = 0;
		status_ = nc_inq_attlen(var.group.group_ID, var.var_ID, &internalName.front(), &attlen);
		vector<char> value(attlen + 1, 0);
		status_ = nc_get_att_text(var.group.group_ID, var.var_ID, &internalName.front(), &value.front());
		HandleError(status_);
		return CleanString(string(value.begin(), value.end()));
	}

	case(DataType::INT) :
	{
		int intValue;
		status_ = nc_get_att_int(var.group.group_ID, var.var_ID, &internalName.front(), &intValue);
		HandleError(status_);
		return CleanString(std::to_string(intValue));
	}

	case(DataType::DOUBLE) :
	{
		double numericValue;
		status_ = nc_get_att_double(var.group.group_ID, var.var_ID, &internalName.front(), &numericValue);
		HandleError(status_);
		return CleanString(std::to_string(numericValue));
	}

	default:
	{
		std::cout << "Unsupported attribute type: " << (char*)attr.type << std::endl;
		return "";
	}
	}
}

string nc_internal::InternalReader::CleanString(string inputStr) const
{
	string str = inputStr;
	str.erase(std::remove(str.begin(), str.end(), '\0'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

nc_internal::InternalReader::~InternalReader()
{

}
