#include "INetCDF.h"
#include <iostream>
#include "driver.h"

//Initialise with error
nc_internal::INetCDF::INetCDF():status_(-1)
{

}


nc_internal::INetCDF::~INetCDF()
{

}

void nc_internal::INetCDF::HandleError(int status) const
{
	if (status != NC_NOERR)
	{
		std::cout << nc_strerror(status) << std::endl;
		dprintf(nc_strerror(status)); //XXX only for testing
		//ToDo Handle Errors
		throw;
	}
}
