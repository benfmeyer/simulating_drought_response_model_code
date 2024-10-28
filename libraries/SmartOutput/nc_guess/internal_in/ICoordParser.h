//////////////////////////////////////////////////////////////////////////////////////
/// \file ICoordParser.h
/// \brief Gridcell coordinate parser dealing with OMA (orthogonal multidimensional array)
/// CVS (coordinate vector scaler) type input
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CoordI.h"
#include <vector>

namespace GuessNC {
	namespace CF {
		class GridcellOrderedVariable;
	}
}

enum class NeighborOrdering
{
	/// Double floor has the following edges order
	// 3			4
	//
	//		 x
	//
	// 1			2
	//
	DoubleFloor,

	/// Double Ceiling has the following edges order
	// 2			1
	//
	//		 x
	//
	// 4			3
	//
	DoubleCeiling
};

class ICoordParser
{
public:
	ICoordParser();
	virtual ~ICoordParser();
protected:
	void GetSpatialResolution(const GuessNC::CF::GridcellOrderedVariable& var);

	std::vector<CoordI> coords;

	double spatial_resolution;
	double spatial_offset_lon;
	double spatial_offset_lat;

	/// Number of lon cells axis
	int nlons;
	/// Number of lat cells axis
	int nlats;
	/// Minimum lon coordinate of input
	double minlon;
	/// Maximum lon coordinate of input
	double maxlon;
	/// Minimum lat coordinate of input
	double minlat;
	/// Maximum lat coordinate of input
	double maxlat;


	int FindCoordIndex(double lon, double lat);
	int FindLonIndex(double lon);
	int FindLatIndex(double lat);

private: 
	bool is_reduced;
	const double RES_ERROR = 1E-10;
	static const NeighborOrdering ORDERING = NeighborOrdering::DoubleFloor;

	struct Neighbor
	{
		Neighbor(const double lon_scale, const double lat_scale):lon_scale(lon_scale),lat_scale(lat_scale)
		{}


		double lon_scale;
		double lat_scale;
	
		/**
		 * \brief This is the comparison function which determines which neighbor gets chosen first
		 * if we have four neighbors of equal distance;t the moment we choose the LEFT-BOTTOM coordinate (-,-)
		 * \param other Neighbor Gridcell to compare with
		 * \return 
		 */
		bool operator < (Neighbor const& other) const;
	};

	std::vector<Neighbor> GetNeighbors(double index_scaled_lon, double index_scaled_lat) const;	



};



