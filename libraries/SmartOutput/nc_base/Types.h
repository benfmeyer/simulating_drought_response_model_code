#pragma once

namespace nc_internal
{
	enum class StorageType
	{
		Chunked,
		Contigues
	};

	enum class DataType
	{
		FLOAT,
		DOUBLE,
		INT,
		TEXT
	};

	enum class DeflationLevel
	{
		None,
		One, Two, Three, Four, Five, Six, Seven, Eight, Nine
	};

	enum class WriterType
	{
		Single,
		Parallel
	};

	enum class DataFormat
	{
		/// Standard 32-bit file format (32 bit variable layout) and filesize < 2GB
		NC3_CDF1,
		/// Advanced 64-bit-offset file format, for files with size > 2GB
		NC3_CDF2,
		/// Advanced 64-bit file format, for files > 2GB and variables > 2GB.
		NC3_CDF5,
		/// Netcdf 4 file with 64bit format
		NC4_HDF5
	};
}
