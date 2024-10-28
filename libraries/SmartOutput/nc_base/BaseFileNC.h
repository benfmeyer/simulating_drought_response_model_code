#pragma once
#include "INetCDF.h"
#include <string>
#include "OpenMode.h"
#include "BaseFile.h"

namespace nc_internal
{
	using std::string;
	using namespace nc_base;

	class BaseFileNC : public INetCDF, public BaseFile
	{
	protected:
		BaseFileNC(string filename);
		//Empty basefile
		BaseFileNC();
		void Init(Mode mode, DataFormat data_format = DataFormat::NC4_HDF5);
		
	public:
		void Close() override;
		virtual ~BaseFileNC();
	};

}
