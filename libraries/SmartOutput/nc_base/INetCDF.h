#pragma once
//No need for checking MPI, if we have ParNetCDF, mpi is already installed
#ifdef HAVE_ParNETCDF
#include <netcdf_par.h>
#include <mpi.h>
#include "driver.h"
#endif 
#include "netcdf.h"

namespace nc_internal
{
	class INetCDF
	{
	public:
		INetCDF();
		virtual ~INetCDF();

	protected:
		int status_;
		void HandleError(int status) const;
	};

}