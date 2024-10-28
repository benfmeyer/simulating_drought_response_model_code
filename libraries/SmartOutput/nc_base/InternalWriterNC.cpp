#include "InternalWriterNC.h"
#include "driver.h"

nc_internal::InternalWriterNC::InternalWriterNC(): BaseFileNC()
{
}

nc_internal::InternalWriterNC::InternalWriterNC(string filename) : BaseFileNC(filename)
{
}

nc_internal::InternalWriterNC::~InternalWriterNC()
{
}

void nc_internal::InternalWriterNC::Reopen(string filename, Mode mode)
{
	filename_ = filename;
	Init(mode);
}

void nc_internal::InternalWriterNC::SetDimensions(vector<Dimension>& dimensions)
{
	for (Dimension& dim : dimensions)
	{
		dimensions_.push_back(&dim);
	}
}

void nc_internal::InternalWriterNC::SetMainGroup(Group& main_group)
{
	main_group_ = &main_group;
}

void nc_internal::InternalWriterNC::SetGroups(vector<Group>& groups)
{
	for (Group& grp : groups)
	{
		groups_.push_back(&grp);
	}
}

void nc_internal::InternalWriterNC::AddGroup(Group& group)
{
	groups_.push_back(&group);
}

void nc_internal::InternalWriterNC::EndDef()
{
	DefineDimensions();
	DefineGroupsVariablesAttributes(*main_group_);

	for (int grp_index = 0; grp_index < groups_.size(); ++grp_index)
	{
		DefineGroupsVariablesAttributes(*groups_[grp_index]);
	}
}

void nc_internal::InternalWriterNC::DefineDimensions()
{
	for (int i = 0; i < dimensions_.size(); ++i)
	{
		AddDimension(*dimensions_[i]);
	}
}

void nc_internal::InternalWriterNC::DefineGroupsVariablesAttributes(Group& group)
{
	AddGroupToList(group);

	//Write global group attributes
	for (int attr_id = 0; attr_id < group.global_attributes.size(); ++attr_id)
	{
		AddGlobalAttribute(group, group.global_attributes[attr_id]);
	}

	for (int var_id = 0; var_id < group.variables.size(); ++var_id)
	{
		AddVariable(group, group.variables[var_id]);
		for (int attr_id = 0; attr_id < group.variables[var_id].attributes.size(); ++attr_id)
		{
			AddAttribute(group, group.variables[var_id], group.variables[var_id].attributes[attr_id]);
		}
	}
}

void nc_internal::InternalWriterNC::AddGroupToList(Group& group)
{
	status_ = nc_def_grp(main_ncid_, group.name.c_str(), &group.group_ID);
	HandleError(status_);
	//parrent_ncid = group.group_ID;
}

void nc_internal::InternalWriterNC::AddDimension(Dimension& dim)
{
	status_ = nc_def_dim(main_ncid_, dim.name.c_str(), dim.count, &dim.index);
	HandleError(status_);
}

void nc_internal::InternalWriterNC::AddVariable(Group& group, Variable& variable)
{

	nc_type nctype;

	switch (variable.type) 
	{
	case DataType::FLOAT:
		nctype = NC_FLOAT;
		break;
	case DataType::DOUBLE:
		nctype = NC_DOUBLE;
		break;
	case DataType::INT:
		nctype = NC_INT;
		break;
	case DataType::TEXT:
		nctype = NC_STRING; //This might be changed back to nc_char one time
		break;		
	default: 
		nctype = NC_INT64;
	}

	status_ = nc_def_var(group.group_ID, variable.name.c_str(), nctype, variable.dim_indicies.size(), &variable.dim_indicies.front(), &variable.var_ID);
	HandleError(status_);

	if (variable.chunk_sizes.size() > 0)
	{
		status_ = nc_def_var_chunking(group.group_ID, variable.var_ID, NC_CHUNKED, &variable.chunk_sizes.front());
		HandleError(status_);
	}

	//Do we have deflation?
	if (variable.def_level != DeflationLevel::None)
	{
		int def = static_cast<int>(variable.def_level);
		status_ = nc_def_var_deflate(group.group_ID, variable.var_ID, 0, 1, def);
		HandleError(status_);
	}

	#ifdef HAVE_ParNETCDF
	status_ = nc_var_par_access(variable.group.group_ID, variable.var_ID, NC_COLLECTIVE);
	HandleError(status_);
	#endif

}

void nc_internal::InternalWriterNC::AddAttribute(Group& group, Variable& variable, Attribute& attribute)
{
	//status_ = nc_put_att_text(group.group_ID, variable.var_ID, attribute.name.c_str(), attribute.value.length(), attribute.value.c_str());
	//HandleError(status_);

	switch (attribute.type)
	{

	case DataType::FLOAT:
	{
		float value_f = std::stof(attribute.value);
		status_ = nc_put_att_float(group.group_ID, variable.var_ID, attribute.name.c_str(), NC_FLOAT, 1, &value_f);
	}
	break;

	case DataType::DOUBLE:
	{
		double value_d = std::stod(attribute.value);
		status_ = nc_put_att_double(group.group_ID, variable.var_ID, attribute.name.c_str(), NC_DOUBLE, 1, &value_d);

	}
	break;

	case DataType::INT:
	{
		int value_i = std::stoi(attribute.value);
		status_ = nc_put_att_int(group.group_ID, variable.var_ID, attribute.name.c_str(), NC_INT, 1, &value_i);
	}
	break;

	case DataType::TEXT:
	{
		status_ = nc_put_att_text(group.group_ID, variable.var_ID, attribute.name.c_str(), attribute.value.length(), attribute.value.c_str());
	}
	break;
	default:
		break;
	}
	HandleError(status_);

}

void nc_internal::InternalWriterNC::AddGlobalAttribute(Group& group, Attribute& attribute)
{
	status_ = nc_put_att_text(group.group_ID, NC_GLOBAL, attribute.name.c_str(), attribute.value.length(), attribute.value.c_str());
	HandleError(status_);
}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const vector<double>& data)
{
	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	switch (variable.type)
	{
	case DataType::DOUBLE:
	{
		status_ = nc_put_vara_double(variable.group.group_ID, variable.var_ID, &range.offsets.front(), &range.counts.front(), &data.front());
		HandleError(status_);
		break;
	}
	case DataType::FLOAT:
	{
		vector<float> dataF(data.begin(), data.end());
		WriteDataInternal(variable, range, dataF);
		break;
	}
	default:
	{
		std::cout << "Variable and To-Write-Data mismatch";
		throw;
	}
	}
}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const vector<float>& data)
{
	vector<int>& indicies = variable.dim_indicies;
	if (variable.type != DataType::FLOAT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
	}

	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	status_ = nc_put_vara_float(variable.group.group_ID, variable.var_ID, &range.offsets.front(), &range.counts.front(), &data.front());
	HandleError(status_);

}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const vector<int>& data)
{
	if (variable.type != DataType::INT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
		throw;
	}
	
	vector<int>& indicies = variable.dim_indicies;

	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	status_ = nc_put_vara_int(variable.group.group_ID, variable.var_ID, &range.offsets.front(), &range.counts.front(), &data.front());
	HandleError(status_);
}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const vector<string>& data)
{
	if (variable.type != DataType::TEXT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
		throw;
	}


	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}


	vector<vector<char> > data_vs(data.size());
	
	const char** data_s = new const char*[data.size()];

	for (size_t i = 0; i < data.size(); i++)
		data_s[i] = new char[data[i].size()];

	for (size_t i = 0; i < data.size(); i++)
		data_s[i] = data[i].c_str();
	
	status_ = nc_put_vara_string(variable.group.group_ID, variable.var_ID, &range.offsets.front(), &range.counts.front(), data_s);
	HandleError(status_);

	//Todo Check for mempory leak at this point ?!
	//status_ = nc_free_string(data.size(), const_cast<char **>(data_s));
	//for (int i = 0; i < data.size(); i++)
	//	delete[] data_s[i];
	delete[] data_s;
}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const double data_point)
{
	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	if (variable.type != DataType::DOUBLE)
	{
		std::cout << "Variable and To-Write-Data mismatch";
		throw;
	}

	status_ = nc_put_var1_double(main_group_->group_ID, variable.var_ID, &range.offsets.front(), &data_point);
	HandleError(status_);
}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const float data_point)
{
	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	if (variable.type != DataType::FLOAT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
	}
	
	status_ = nc_put_var1_float(main_group_->group_ID, variable.var_ID, &range.offsets.front(), &data_point);
	HandleError(status_);

}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const int data_point)
{
	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	if (variable.type != DataType::INT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
	}

	status_ = nc_put_var1_int(main_group_->group_ID, variable.var_ID, &range.offsets.front(), &data_point);
	HandleError(status_);
}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const size_t data_point)
{
	vector<int>& indicies = variable.dim_indicies;
	int value = static_cast<int> (data_point);
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	if (variable.type != DataType::INT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
	}

	status_ = nc_put_var1_int(main_group_->group_ID, variable.var_ID, &range.offsets.front(), &value);
	HandleError(status_);
}

void nc_internal::InternalWriterNC::WriteDataInternal(Variable& variable, const Range& range, const string data_point)
{
	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	if (variable.type != DataType::TEXT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
	}

	status_ = nc_put_var1_text(main_group_->group_ID, variable.var_ID, &range.offsets.front(), &data_point.front());
	HandleError(status_);
}

void nc_internal::InternalWriterNC::WriteDataInternalNC3(Variable& variable, const Range& range, vector<float> data)
{
	vector<int>& indicies = variable.dim_indicies;
	if (variable.type != DataType::FLOAT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
	}

	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	status_ = nc_put_vara_float(main_group_->group_ID, variable.var_ID, &range.offsets.front(), &range.counts.front(), &data.front());
	HandleError(status_);
}

void nc_internal::InternalWriterNC::WriteDataInternalNC3(Variable& variable, const Range& range, vector<string> data)
{
	if (variable.type != DataType::TEXT)
	{
		std::cout << "Variable and To-Write-Data mismatch";
		throw;
	}
	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	const char** data_s = new const char*[data.size()];
	for (size_t i = 0; i < data.size(); i++)
		data_s[i] = new char[data[i].size()];

	for (size_t i = 0; i < data.size(); i++)
		data_s[i] = data[i].c_str();

	status_ = nc_put_vara_string(main_group_->group_ID, variable.var_ID, &range.offsets.front(), &range.counts.front(), data_s);
	HandleError(status_);

	//Todo do we have a memory leak here?
	//status_ = nc_free_string(data.size(), const_cast<char **>(data_s));
	//for (int i = 0; i < data.size(); i++)
	//	delete[] data_s[i];
	delete[] data_s;
}
