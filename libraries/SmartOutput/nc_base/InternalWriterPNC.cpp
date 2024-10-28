#include "InternalWriterPNC.h"
#include "driver.h"

#ifdef HAVE_PNETCDF

nc_internal::InternalWriterPNC::InternalWriterPNC()
{
}

nc_internal::InternalWriterPNC::InternalWriterPNC(string filename): BaseFilePNC(filename), firstOutput_(true),
                                                                    n_reps_(0), var_index_float(0), var_index_int(0),
                                                                    var_index_total(0),
                                                                    bbufsize_(0)
{
}


nc_internal::InternalWriterPNC::~InternalWriterPNC()
{
}

void nc_internal::InternalWriterPNC::Reopen(string filename, Mode mode)
{
	string message = "Reopen is not supported in PNetCDF at the moment.";
	throw(message);
}

void nc_internal::InternalWriterPNC::SetDimensions(vector<Dimension>& dimensions)
{
	for (Dimension& dim : dimensions)
	{
		dimensions_.push_back(&dim);
	}
}

void nc_internal::InternalWriterPNC::SetMainGroup(Group& main_group)
{
	main_group_ = &main_group;
}

void nc_internal::InternalWriterPNC::SetGroups(vector<Group>& groups)
{
	for (Group& grp : groups)
	{
		groups_.push_back(&grp);
	}
}

void nc_internal::InternalWriterPNC::AddGroup(Group& group)
{
	groups_.push_back(&group);
}

void nc_internal::InternalWriterPNC::EndDef()
{
	DefineDimensions();
	DefineGroupsVariablesAttributes(*main_group_);

	for (int grp_index = 0; grp_index < groups_.size(); ++grp_index)
	{
		DefineGroupsVariablesAttributes(*groups_[grp_index]);
	}

	file_->enddef();
}

Group& nc_internal::InternalWriterPNC::GetGroup(string name)
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
	std::cout << "Could not find group with name: " << name << std::endl;
	throw;
}

Variable& nc_internal::InternalWriterPNC::GetVariable(string grpname, string varname)
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
	std::cout << "Could not find variable with name: " << varname << std::endl;
	throw;
}


void nc_internal::InternalWriterPNC::DefineDimensions()
{
	//do not use push back here!
	ncmpi_dims_.resize(dimensions_.size());

	for (int i = 0; i < dimensions_.size(); ++i)
	{
		Dimension* dim = dimensions_[i];

		ncmpi_dims_[i] = file_->addDim(dim->name, dim->count);

		//AddDimension(*dimensions_[i]);
	}
}

void nc_internal::InternalWriterPNC::DefineGroupsVariablesAttributes(Group& group)
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

void nc_internal::InternalWriterPNC::AddGroupToList(Group& group)
{
	//Todo Not supported yet by PNetCDF 1.8.1!
	//NcmpiGroup&& ncmpi_group = file_->addGroup(group.name);
	//ncmpi_def_grp(file_->getId(), group.name.c_str(), &group.group_ID);

	//Check Variables and replace _ with -. 
	//This is done to avoid post processing problems, as PNETCDF does not supprot NC4-Grouping...
	group.name = ReplaceUnderscore(group.name);
}

void nc_internal::InternalWriterPNC::AddDimension(Dimension& dim)
{
	//NcmpiDim dim_pnc = file_->addDim(dim.name, dim.count);
	//ncmpi_dims_.push_back(dim_pnc);
}

void nc_internal::InternalWriterPNC::AddVariable(Group& group, Variable& variable)
{
	//Check Variables and replace _ with -. 
	//This is done to avoid post processing problems, as PNETCDF does not supprot NC4-Grouping...
	variable.name = ReplaceUnderscore(variable.name);

	int ndims = variable.dim_indicies.size();
	vector<NcmpiDim> dimid_local;

	for (int i = 0; i < ndims; ++i)
	{
		int id = variable.dim_indicies[i];
		dimid_local.push_back(ncmpi_dims_[id]);
	}		

	// Todo Not supported by Pnetcdf yet !
	//cout << " Getting Group...";
	//NcmpiGroup ncmpi_group = file_->getGroup(group.name);
	//cout << "Done!" << endl;

	NcmpiType::ncmpiType ncmpitype;

	switch (variable.type)
	{
	case DataType::FLOAT:
		ncmpitype = NcmpiType::ncmpi_FLOAT;
		break;
	case DataType::DOUBLE:
		ncmpitype = NcmpiType::ncmpi_DOUBLE;
		break;
	case DataType::INT:
		ncmpitype = NcmpiType::ncmpi_INT;
		break;
	case DataType::TEXT:
		ncmpitype = NcmpiType::ncmpi_CHAR;
		break;
	default:
		ncmpitype = NcmpiType::ncmpi_INT64;
		
	}	
	ncmpi_vars_.push_back(file_->addVar(group.name + "_" + variable.name, ncmpitype, dimid_local));
	//ncmpi_group.addVar(variable.name, ncmpitype, dimid_local);
}

void nc_internal::InternalWriterPNC::AddAttribute(Group& group, Variable& variable, Attribute& attribute)
{
	NcmpiVar&& var = file_->getVar(group.name + "_" + variable.name);


	if (attribute.name == "_FillValue")
	{
		float value_f = std::stof(attribute.value);
		var.putAtt(attribute.name, NcmpiType::ncmpi_FLOAT, value_f);
	}
	else
	{
		var.putAtt(attribute.name, attribute.value);
	}
}

void nc_internal::InternalWriterPNC::AddGlobalAttribute(Group& group, Attribute& attribute)
{
	if (attribute.name == "_FillValue")
	{
		float value_f = std::stof(attribute.value);
		file_->putAtt(attribute.name, NcmpiType::ncmpi_FLOAT, value_f);
	}
	else
	{
		file_->putAtt(attribute.name, attribute.value);
	}
	
}

void nc_internal::InternalWriterPNC::WriteDataInternal(Variable& variable, const Range& range, const vector<double>& data)
{

	vector<int>& indicies = variable.dim_indicies;
	if (indicies.size() != range.offsets.size())
	{
		std::cout << "Indicies of variable and ranges mismatch!" << std::endl;
		throw;
	}

	//NcmpiVar ncmpi_var = file_->getGroup(variable.group.name).getVar(variable.name);
	//NcmpiVar ncmpi_var =  file_->getVar(variable.group.name + "_" + variable.name);

	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);


	//ncmpi_vars_.push_back(file_->getVar(variable.group.name + "_" + variable.name));

	vector<MPI_Offset> mpi_offset;
	for (int val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (int val : range.counts)
		mpi_counts.push_back(val);

	switch (variable.type)
	{
	case DataType::DOUBLE:
	{
		ncmpi_var.putVar_all(mpi_offset, mpi_counts, &data.front());
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


void nc_internal::InternalWriterPNC::WriteDataInternal(Variable& variable, const Range& range, const vector<float>& data)
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

	//NcmpiVar ncmpi_var = file_->getGroup(variable.group.name).getVar(variable.name);
	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);



	vector<MPI_Offset> mpi_offset;
	for (int val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (int val : range.counts)
		mpi_counts.push_back(val);

	ncmpi_var.putVar_all(mpi_offset, mpi_counts, &data.front());
}

void nc_internal::InternalWriterPNC::WriteDataInternal(Variable& variable, const Range& range, const vector<int>& data)
{
	//NcmpiVar ncmpi_var = file_->getGroup(variable.group.name).getVar(variable.name);
	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);

	vector<MPI_Offset> mpi_offset;
	for (size_t val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (size_t val : range.counts)
		mpi_counts.push_back(val);

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
	ncmpi_var.putVar_all(mpi_offset, mpi_counts, &data.front());
}


void nc_internal::InternalWriterPNC::StartIndependent(const int number_of_total_variabls)
{

	int id = file_->getId();
	int err = ncmpi_begin_indep_data(id);
	dprintf(ncmpi_strerror(err));


	ncmpi_end_indep_data(id);
	//if (!firstOutput_)
	//{
	//	file_->Wait_all(n_vars_float_, &req_.front(), &st_.front());
	//	//file_->Buffer_detach();
	//	buffer_float_.clear();
	//	buffer_vars_.clear();
	//	buffer_ranges_.clear();
	//}


	////Set total number of independend sends
	//n_vars_float_ = number_of_total_variabls;
	////Initialise var index with zero to reset the counter
	//var_index_float = 0;


	//st_.resize(number_of_total_variabls);
	//req_.resize(number_of_total_variabls);

	//buffer_float_.clear();
	//buffer_float_.resize(number_of_total_variabls);
	//for (vector<float>& row : buffer_float_)
	//{
	//	row.resize(113);
	//}

	////bbufsize_ = 113 * number_of_total_variabls * sizeof(float);
	////file_->Buffer_attach(bbufsize_);




	////THIS WORKS needed for Independed mode
	////int id = file_->getId();
	////int err = ncmpi_begin_indep_data(id);
	////dprintf(ncmpi_strerror(err));
	////
}
void nc_internal::InternalWriterPNC::WriteDataInternalIndependend(Variable& variable, const Range& range,
	const vector<int>& data)
{
	int id = file_->getId();
	int err = ncmpi_begin_indep_data(id);
	dprintf(ncmpi_strerror(err));
	
	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);
	vector<MPI_Offset> mpi_offset;
	for (size_t val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (size_t val : range.counts)
		mpi_counts.push_back(val);

	ncmpi_var.putVar(mpi_offset, mpi_counts, &data.front());

	ncmpi_end_indep_data(id);
	dprintf(ncmpi_strerror(err));

	//ncmpi_vars_.push_back(ncmpi_var);
	//buffer_float_[var_index_float] = data;
	//ncmpi_var.iputVar(mpi_offset, mpi_counts, &data.front(), &req_[var_index_float]);

}
void nc_internal::InternalWriterPNC::WriteDataInternalIndependend(Variable& variable, const Range& range,
	const vector<float>& data)
{

	int id = file_->getId();
	int err = ncmpi_begin_indep_data(id);
	dprintf(ncmpi_strerror(err));

	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);
	vector<MPI_Offset> mpi_offset;
	for (size_t val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (size_t val : range.counts)
		mpi_counts.push_back(val);

	ncmpi_var.putVar(mpi_offset, mpi_counts, &data.front());

	ncmpi_end_indep_data(id);
	dprintf(ncmpi_strerror(err));


	//NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);
	//buffer_vars_.push_back(ncmpi_var);
	//buffer_ranges_.push_back(range);

	//vector<MPI_Offset> mpi_offset;
	//for (size_t val : range.offsets)
	//	mpi_offset.push_back(val);

	//vector<MPI_Offset> mpi_counts;
	//for (size_t val : range.counts)
	//	mpi_counts.push_back(val);

	//buffer_float_[var_index_float] = data;

	//MPI_Offset** starts = new MPI_Offset*[1];
	//starts[0] = new MPI_Offset[2];

	//starts[0][0] = mpi_offset[0];
	//starts[0][1] = mpi_offset[1];

	//MPI_Offset** counts = new MPI_Offset*[1];
	//counts[0] = new MPI_Offset[2];

	//counts[0][0] = mpi_counts[0];
	//counts[0][1] = mpi_counts[1];

	//const int num_reqs = 1;

	////THIS WORKS
	//////int err = ncmpi_put_varn_float(file_->getId(), ncmpi_var.getId(), num_reqs, starts, counts, &data.front());
	//////dprintf(ncmpi_strerror(err));


	////ncmpi_var.putVarn(1, &mpi_offset, &mpi_counts, &buffer_float_[var_index_float].front());

	////This works
	////ncmpi_var.putVarn(num_reqs, starts, counts, &data.front());

	////New try Works without wait
	////ncmpi_var.putVar(mpi_offset, mpi_counts, &data.front());

	////ncmpi_var.bputVar(mpi_offset, mpi_counts, &req_[var_index_float],);

	////works not so well
	////buffer_float_[var_index_float] = data;
	////ncmpi_var.bputVar(mpi_offset, mpi_counts, &buffer_float_[var_index_float].front(), &req_[var_index_float]);
	////ncmpi_var.iputVar(mpi_offset, mpi_counts, &buffer_float_[var_index_float].front(), &req_[var_index_float]);
	////ncmpi_var.iputVar(mpi_offset, mpi_counts, &buffer_float_[var_index_float].front(), &req_[var_index_float]);
	////ncmpi_var.iputVar(&data.front(), mpi_offset, mpi_counts, );	

	////Add up index
	//var_index_float++;
	//std::cout << "Var index: " << var_index_float << "\n";
}

void nc_internal::InternalWriterPNC::WriteDataInternalIndependend(Variable& variable, const Range& range,
	const vector<double>& data)
{
	int id = file_->getId();
	int err = ncmpi_begin_indep_data(id);
	dprintf(ncmpi_strerror(err));

	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);
	vector<MPI_Offset> mpi_offset;
	for (size_t val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (size_t val : range.counts)
		mpi_counts.push_back(val);

	ncmpi_var.putVar(mpi_offset, mpi_counts, &data.front());

	ncmpi_end_indep_data(id);
	dprintf(ncmpi_strerror(err));
}

void nc_internal::InternalWriterPNC::WriteDataInternalIndependend(Variable& variable, const Range& range, const string& data)
{
	int id = file_->getId();
	int err = ncmpi_begin_indep_data(id);
	dprintf(ncmpi_strerror(err));

	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);
	vector<MPI_Offset> mpi_offset;
	for (size_t val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (size_t val : range.counts)
		mpi_counts.push_back(val);

	ncmpi_var.putVar(mpi_offset, mpi_counts, &data.front());

	ncmpi_end_indep_data(id);
	dprintf(ncmpi_strerror(err));
}


string nc_internal::InternalWriterPNC::ReplaceUnderscore(string text) const
{
	string s = text;
	replace(text.begin(), text.end(), '_', '-');
	return s;
}

void nc_internal::InternalWriterPNC::WriteDataInternalFullBuffered(Variable& variable, const Range& range, const vector<double>& data)
{
	vector<float> dataF(data.begin(), data.end());
	WriteDataInternalFullBuffered(variable, range, dataF);
}

void nc_internal::InternalWriterPNC::WriteDataInternalFullBuffered(Variable& variable, const Range& range, const vector<float>& data)
{
	float_buffer_.SetDataBuffer(data, var_index_float);

	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);

	vector<MPI_Offset> mpi_offset;
	for (size_t val : range.offsets)
	{
		mpi_offset.push_back(val);
	}
	

	vector<MPI_Offset> mpi_counts;
	for (size_t val : range.counts)
	{
		mpi_counts.push_back(val);
	}

	//std::cout <<variable.name << " var_index_float: " <<  var_index_float <<  " "<< float_buffer_.GetNVars() << "\n";

	ncmpi_var.bputVar(mpi_offset, mpi_counts, &float_buffer_.GetDataBuffer(var_index_float).front(), &req_[var_index_total]);

	var_index_float++;
	var_index_total++;
}

void nc_internal::InternalWriterPNC::WriteDataInternalFullBuffered(Variable& variable, const Range& range,
	const vector<int>& data)
{
	//std::cout << "int data size" << data.size() << std::endl;

	//std::cout << " var_index_int:  " << var_index_int;
	//std::cout << " data is " << data[0];

	int_buffer_.SetDataBuffer(data, var_index_int);

	
	//std::cout << " 1 is " << data[0];


	NcmpiVar& ncmpi_var = FindVariable(variable.group.name + "_" + variable.name);

	//std::cout << " 2 is " << data[0];

	vector<MPI_Offset> mpi_offset;
	for (size_t val : range.offsets)
		mpi_offset.push_back(val);

	vector<MPI_Offset> mpi_counts;
	for (size_t val : range.counts)
		mpi_counts.push_back(val);


	//std::cout << variable.name << " var_index_int: " << var_index_int << " " << int_buffer_.GetNVars() << "\n";

	ncmpi_var.bputVar(mpi_offset, mpi_counts, &int_buffer_.GetDataBuffer(var_index_int).front(), &req_[var_index_total]);

	var_index_int++;
	var_index_total++;
}

void nc_internal::InternalWriterPNC::FullBufferedEnd()
{
	//std::cout << " FINAL Wait: for float-reqs  req:";
	//for (int i = 0; i < req_.size(); ++i)
	//{
	//	std::cout << req_[i] << " ";
	//}

	//std::cout << std::endl;
	file_->Wait_all(var_index_total, &req_.front(), &st_.front());
	//std::cout << " Detach buffer " << std::endl;
	file_->Buffer_detach();

	//std::cout << " FINAL:";

	for (int i = 0; i < var_index_total; i++)
	{
		if (st_[i] != NC_NOERR)
			dprintf("Error: nonblocking write fails on request %d (%s)\n",
				i, ncmpi_strerror(st_[i]));
	}

}

void nc_internal::InternalWriterPNC::EndIndependent()
{
	//for (int i = 0; i < buffer_vars_.size(); ++i)
	//{
	//	Range& range = buffer_ranges_[i];


	//	vector<MPI_Offset> mpi_offset;
	//	for (size_t val : range.offsets)
	//		mpi_offset.push_back(val);

	//	vector<MPI_Offset> mpi_counts;
	//	for (size_t val : range.counts)
	//		mpi_counts.push_back(val);

	//	MPI_Offset** starts = new MPI_Offset*[1];
	//	starts[0] = new MPI_Offset[2];

	//	starts[0][0] = mpi_offset[0];
	//	starts[0][1] = mpi_offset[1];

	//	MPI_Offset** counts = new MPI_Offset*[1];
	//	counts[0] = new MPI_Offset[2];

	//	counts[0][0] = mpi_counts[0];
	//	counts[0][1] = mpi_counts[1];

	//	//Tasdasdasdhis works
	//	//ncmpi_var.putVarn(num_reqs, starts, counts, &data.front());

	//	//Try this again
	//	buffer_vars_[i].iputVar(mpi_offset, mpi_counts, &buffer_float_[i].front(), &req_[i]);
	//}

	//file_->Wait(n_vars_float_, &req_.front(), &st_.front());
	//file_->Buffer_detach();
	//buffer_float_.clear();
	//buffer_vars_.clear();
	//buffer_ranges_.clear();

	//for (int i = 0; i < n_vars_float_; i++)
	//{
	//	if (st_[i] != NC_NOERR)
	//		dprintf("Error: nonblocking write fails on request %d (%s)\n",
	//			i, ncmpi_strerror(st_[i]));
	//}

	//if (firstOutput_)
	//{
	//	firstOutput_ = false;
	//}

	//int id = file_->getId();
	////int err = ncmpi_end_indep_data(id);
	////dprintf(ncmpi_strerror(err));
}

void nc_internal::InternalWriterPNC::SetFloatBuffer(const long n_vars, const long n_reps, const vector<long> buf_sizes)
{
	float_buffer_ = { n_reps, n_vars , buf_sizes };	
	
	//std::cout << "n_vars_float: " << n_vars << std::endl;
	//std::cout << "n_reps: " << n_reps << std::endl;
}

void nc_internal::InternalWriterPNC::SetIntBuffer(const long n_vars, const long n_reps,
	const vector<long> buf_sizes)
{

	int_buffer_ = { n_reps, n_vars , buf_sizes };

	//std::cout << "n_vars_int: " << n_vars << std::endl;
	//std::cout << "n_reps: " << n_reps << std::endl;
}



void nc_internal::InternalWriterPNC::AttachBuffer()
{
	bbufsize_ = 0;
	req_.clear();
	st_.clear();


	bbufsize_ += int_buffer_.GetBBuffer();
	bbufsize_ += float_buffer_.GetBBuffer();

	req_.resize(int_buffer_.GetSize() + float_buffer_.GetSize());
	st_.resize(int_buffer_.GetSize() + float_buffer_.GetSize());

	//std::cout << "Bufsize: " << bbufsize_ << std::endl;

	file_->Buffer_attach(bbufsize_);
}

NcmpiVar& nc_internal::InternalWriterPNC::FindVariable(const string name)
{
	for (int i = 0; i < ncmpi_vars_.size(); ++i)
	{
		if (ncmpi_vars_[i].getName() == name)
		{
			return ncmpi_vars_[i];
		}
	}
	std::cout << "Variable not found!" << std::endl;
	throw;
}
#endif
