#ifdef HAVE_PNETCDF
#include "WriterPNC.h"

WriterPNC::WriterPNC(string filename, Mode openmode, MPI_Comm& comm): BaseFilePNC(filename), InternalWriterPNC(filename)
{
	// Call the BaseFile-Initializer. Resharper thinks that the Basefile qualifier is redundant
	// however this is not the case, at least not in C++14!
	BaseFilePNC::Init(openmode, comm);
}

WriterPNC::WriterPNC()
{

}

WriterPNC::~WriterPNC()
{
	//Nothing to do her as the base class desstructor will get called automatically
}

void WriterPNC::Close()
{
	BaseFilePNC::Close();
}
#endif
