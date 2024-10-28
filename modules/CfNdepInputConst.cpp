#include <netcdf.h>
#include <parameters.h>
#include <iostream>
#include "guessnc.h"
#include "cf.h"
#include "CfNdepInputConst.h"


CfNdepInputConst::CfNdepInputConst() : netcdf_noy_id(-1),  netcdf_nhx_id(-1) {}
CfNdepInputConst::~CfNdepInputConst() = default;

void CfNdepInputConst::init() {
    netcdf_noy_id = GuessNC::open_ncdf(param["file_mip_noy"].str);
    netcdf_nhx_id = GuessNC::open_ncdf(param["file_mip_nhx"].str);
}


double CfNdepInputConst::get_ndep_noy(double lon, double lat) {
    return get_ndep(lon, lat, NOY);
}

double CfNdepInputConst::get_ndep_nhx(double lon, double lat) {
    return get_ndep(lon, lat, NHX);
}


double CfNdepInputConst::get_ndep(double lon, double lat, CfNdepInputConst::NDEP_TYPE ndep_type) {
    int ncid;
    if(ndep_type == NHX){
        ncid = netcdf_nhx_id;
    } else {
        ncid = netcdf_noy_id;
    }

    double ndep;

    auto lon_idx = (size_t) (lon + 179.5);
    auto lat_idx = (size_t) (lat + 89.5);
    const size_t index[] = {lat_idx, lon_idx};

    int status = nc_get_var1_double(ncid, 2, index, &ndep);

    if(status != 0){
        std::cout << "Failed reading NDEP, status " << status << std::endl;
        throw GuessNC::CF::CFError("Couldn't read NDEP file!");
    }

    return ndep;
}
