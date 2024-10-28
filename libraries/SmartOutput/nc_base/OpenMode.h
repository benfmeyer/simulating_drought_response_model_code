#pragma once
namespace nc_base
{
	enum class Mode
	{
		ReadOnly,
		Create,
		CreateParallel,
		OpenSingle,
		OpenMulti, 
		OpenParallel
	};


	enum class PNetCDFFileFormat
	{
		// Classic format, classic data model
		NC3_Classic_32,
		// 64-bit offset format, classic data model
		NC3_Classic_64,
		// netCDF-4/HDF5 format, enhanced data model
		NC4_Enhanced_32,
		// netCDF-4/HDF5 format, classic data model
		NC4_Classic_32,
		// CDF-5 format, classic data model
		CDF5_Classic_64,
	};
}