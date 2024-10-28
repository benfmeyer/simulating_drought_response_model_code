#include "BaseFileNC.h"
#include <iostream>

nc_internal::BaseFileNC::BaseFileNC(string filename):BaseFile(filename)
{
}

nc_internal::BaseFileNC::BaseFileNC(): BaseFile()
{
}

void nc_internal::BaseFileNC::Init(const Mode mode, const DataFormat data_format)
{
	file_format_ = data_format;

	// Set default format to netcdf 4
	int nc_format = NC_NETCDF4;

	switch (file_format_)
	{
	case DataFormat::NC3_CDF1: 
		nc_format = NC_CLASSIC_MODEL;
		break;
	case DataFormat::NC3_CDF2: 
		nc_format = NC_64BIT_OFFSET;
		break;
	case DataFormat::NC3_CDF5:
		nc_format = NC_64BIT_OFFSET;
		break;
	case DataFormat::NC4_HDF5:
		nc_format = NC_NETCDF4;
		break;
	default: ;
	}

	switch (mode)
	{
	case Mode::ReadOnly:
		status_ = nc_open(&filename_.front(), nc_format | NC_NOWRITE, &main_ncid_);
		break;
	case Mode::Create:
		status_ = nc_create(&filename_.front(), nc_format | NC_WRITE, &main_ncid_);
		break;
	case Mode::OpenSingle:
		status_ = nc_open(&filename_.front(), nc_format | NC_WRITE, &main_ncid_);
		break;
	case Mode::OpenMulti:
		status_ = nc_open(&filename_.front(), nc_format | NC_WRITE | NC_SHARE, &main_ncid_);
		break;
	case Mode::OpenParallel:
		std::cout << "Cannot use OpenParallel in ncs-mode" << std::endl;
		throw;
	case Mode::CreateParallel:
		std::cout << "Cannot use CreateParallel in ncs-mode" << std::endl;
		throw;
	default:
		throw("Invalid open type");
		break;
	}

	HandleError(status_);
	parrent_ncid = main_ncid_;

}

void nc_internal::BaseFileNC::Close()
{
	status_ = nc_close(main_ncid_);
	HandleError(status_);
}

nc_internal::BaseFileNC::~BaseFileNC()
{
}

