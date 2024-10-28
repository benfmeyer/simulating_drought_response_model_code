#pragma once
#ifdef HAVE_PNETCDF

#include <string>
#include "OpenMode.h"
#include "BaseFile.h"
#include <iostream>
#include "Range.h"
#include <mpi.h>
#include <pnetcdf>

using std::string;
using namespace nc_base;

using namespace PnetCDF;
using namespace PnetCDF::exceptions;


namespace nc_internal
{
	class BaseFilePNC : public BaseFile
	{
	protected:
		BaseFilePNC(string filename);
		//Empty basefile
		BaseFilePNC();
		void SetFileFormat(PNetCDFFileFormat format);
		void Init(Mode mode, MPI_Comm& comm);

		//NcmpiFile::FileFormat file_format;

		MPI_Info info_;
		NcmpiFile* file_;
#ifdef HAVE_ParNETCDF
		MPI_Info info;
#endif 

	public:
		void Close() override;
		virtual ~BaseFilePNC();
	};

}

#endif