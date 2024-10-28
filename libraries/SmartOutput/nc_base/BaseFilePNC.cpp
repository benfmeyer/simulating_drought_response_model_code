#ifdef HAVE_PNETCDF
#include "BaseFilePNC.h"
#include <iostream>

nc_internal::BaseFilePNC::BaseFilePNC(string filename):BaseFile(filename)
{
}

nc_internal::BaseFilePNC::BaseFilePNC():BaseFile()
{
}

void nc_internal::BaseFilePNC::SetFileFormat(PNetCDFFileFormat format)
{
	//file_format = NcmpiFile::classic2;
}

void nc_internal::BaseFilePNC::Init(Mode mode, MPI_Comm& comm)
{
	MPI_Info info;
	//int psizes[2];
	//for (int i=0; i<2; i++) psizes[i] = 0;
	//MPI_Dims_create(10, 2, psizes);	
    MPI_Info_create(&info);
    MPI_Info_set(info, (char*)"nc_var_align_size", (char*)"1");
	
	//ncmpi_set_chunk_cache()
	
	switch (mode) {
	case Mode::CreateParallel:
		try
		{

			/* create a new file for writing ------------------------------------*/
			file_ = new NcmpiFile(comm, filename_, NcmpiFile::replace, NcmpiFile::classic2, info);

			MPI_Info_free(&info);
		}
		catch (NcmpiException& e) {
			std::cout << e.what() << " error code=" << e.errorCode() << " Error!\n";
		}
		break;
	case Mode::OpenParallel:
		try
		{
		/* open an existing file for writing ------------------------------------*/
			file_ = new NcmpiFile(comm, filename_, NcmpiFile::write, NcmpiFile::classic2, info);
			MPI_Info_free(&info);
		}
		catch (NcmpiException& e) {
			std::cout << e.what() << " error code=" << e.errorCode() << " Error!\n";
		}
		break;

		default:
		{			std::cout << "Invalid pnetcdf-type" << std::endl;
			throw;
		}
	}
}

void nc_internal::BaseFilePNC::Close()
{

}

nc_internal::BaseFilePNC::~BaseFilePNC()
{
	delete file_;
}



#endif
