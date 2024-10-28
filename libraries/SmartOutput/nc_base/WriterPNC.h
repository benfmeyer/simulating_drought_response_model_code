#pragma once
#ifdef HAVE_PNETCDF
#include "InternalWriterPNC.h"
namespace nc_base
{
	using namespace nc_internal;

	class WriterPNC : public virtual InternalWriterPNC
	{
	public:
		WriterPNC(string filename, Mode openmode, MPI_Comm& comm);

		//Empty writer
		WriterPNC();
		~WriterPNC();

		void Close() override;
	};
}
#endif
