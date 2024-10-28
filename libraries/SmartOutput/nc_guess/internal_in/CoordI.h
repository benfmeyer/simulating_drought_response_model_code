//////////////////////////////////////////////////////////////////////////////////////
/// \file CoordI.h
/// \brief Coordinate class extendind the stadrd coordinates with further information
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
struct CoordI {

	CoordI(): lon(0.0), lon_index(0), lat(0), lat_index(0), load(0), landid(0), index(0), succes(false)
	{
	}

	CoordI(double lon, double lat): load(-1.0), landid(0), index(0), succes(false), lon_index(0), lat_index(0)
	{
		this->lon = lon;
		this->lat = lat;
	}


	/// decimal longitude
	double lon;
	/// longitude index (only valid for CVS)
	int lon_index;
	/// decimal latitude
	double lat;
	/// latitude index (only valid for CVS)
	int lat_index;
	/// expected load for this coordinate (only valid if load balancing is turned on)
	double load;
	/// index of this coordinate of the coordinate list of the input driver files (only valid for OMA)
	int landid;
	/// index of this coordinate 
	int index;
	/// was LPJ-GUESS successfull when trying to simulate that coord
	bool succes;

	std::string descrip;
};