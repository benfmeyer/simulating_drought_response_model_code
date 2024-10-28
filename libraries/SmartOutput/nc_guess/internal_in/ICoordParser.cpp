#include "ICoordParser.h"
#include "cfvariable.h"
#include "externalinput.h"
#include <cmath>


ICoordParser::ICoordParser()
{
}


ICoordParser::~ICoordParser()
{
}

void ICoordParser::GetSpatialResolution(const GuessNC::CF::GridcellOrderedVariable& var)
{
	is_reduced = var.is_reduced();

	if (!is_reduced)
	{
		std::vector<double> lons = var.get_all_lons();
		std::vector<double> lats = var.get_all_lats();

		if (lons.size() == 0)
		{
			fail("No longitude coords supplied!");
		}
		else if(lons.size() == 1)
		{
			spatial_resolution = 0.0;
		}
		else
		{
			spatial_resolution = fabs(lons[0] - lons[1]);
		}

		nlons = lons.size();
		minlon = lons.front();
		maxlon = lons.back();

		nlats = lats.size();
		minlat = lats.front();
		maxlat = lats.back();

	}	

	else
	{
		coords = var.get_all_coords();


		//Todo move this out of here!
		minlon = coords.front().lon;
		maxlon = coords.front().lon;
		minlat = coords.front().lat;
		maxlat = coords.front().lat;

		for (const CoordI& coord : coords)
		{
			if (minlon > coord.lon)
				minlon = coord.lon;

			if (maxlon < coord.lon)
				maxlon = coord.lon;

			if (minlat > coord.lat)
				minlat = coord.lat;

			if (maxlat < coord.lat)
				maxlat = coord.lat;
		}



		//If there is only one coord in the data we have no resolution
		if (coords.size() == 1)
		{
			spatial_resolution = 0.0;
			spatial_offset_lon = 0.0;
			spatial_offset_lat = 0.0;
			nlats = 1;
			nlons = 1;
		}

		else
		{
			CoordI first = coords[0];
			double min = 1E34;

			for (size_t i = 1; i < coords.size(); i++)
			{
				const double x = (coords[i].lon - first.lon);
				const double y = (coords[i].lat - first.lat);

				const double dist = sqrt(x*x + y*y);

				if (dist < min)
				{
					if (dist != 0.0)
					{
						min = dist;
					}
				}
					
			}

			if (min == 0.0)
				dprintf("Invalid spatial resolution, are some coords twice in list?!");

			spatial_resolution = min;
			const int a = static_cast<int>(coords[0].lon / min);
			spatial_offset_lon = min*a - coords[0].lon > 0 ? min*a - coords[0].lon : -min*a + coords[0].lon;
			const int b = static_cast<int>(coords[0].lat / min);
			spatial_offset_lat = min*b - coords[0].lat > 0 ? min*b - coords[0].lat : -min*b + coords[0].lat;



			nlons = (maxlon - minlon) / spatial_resolution;
			nlats = (maxlat - minlat) / spatial_resolution;

		}
	}

}


int ICoordParser::FindCoordIndex(const double lon, const double lat)
{
		//Find index with floating point precision
		const double index_scaled_lon = (lon - spatial_offset_lon) / spatial_resolution;
		const double index_scaled_lat = (lat - spatial_offset_lat) / spatial_resolution;

		std::vector<Neighbor> possible_neighbors = GetNeighbors(index_scaled_lon, index_scaled_lat);

		// Sort the neighbors to be sure they are in the right order
		// The sorting algorithm is determined by the > operator of neighbor struct
		// Only sort if we have more than 1 element...
		if (possible_neighbors.size() > 1)
		{
			switch (ORDERING)
			{
			case NeighborOrdering::DoubleFloor:
				std::sort(possible_neighbors.begin(), possible_neighbors.end());
				break;
			case NeighborOrdering::DoubleCeiling:
				std::sort(possible_neighbors.begin(), possible_neighbors.end());
				std::reverse(possible_neighbors.begin(), possible_neighbors.end());
				break;
			}
		}


		bool found = false;
		int neighbor_index = 0;
		int id = -1;

		// Loop through the neigbors and find a suitable neighbor
		// Here, a major role plays the index of operator overload of neighbor struct overload
		while (!found && neighbor_index < possible_neighbors.size())
		{
			// Calculate lon and lat which suite gridlist resolution
			const double lon_gridlist = possible_neighbors[neighbor_index].lon_scale*spatial_resolution + spatial_offset_lon;
			const double lat_gridlist = possible_neighbors[neighbor_index].lat_scale*spatial_resolution + spatial_offset_lat;

			// If the distance between lon (of interest) and the nearest neighbor lon_gridlist is greater than the spatial resolution
			if (fabs(fabs(lon_gridlist) - fabs(lon)) > spatial_resolution || fabs(fabs(lat_gridlist) - fabs(lat)) > spatial_resolution)
				dprintf("Warning: Coord (%f,%f) differs too much from nearest neighbor (%f,%f)", lon, lat, lon_gridlist, lat_gridlist);

			int i = 0;
			// Search for the correct lon and lat inside the coord list
			// If no coord is in the list - 1 is returned
			while (!found && i < coords.size())
			{
				const bool lon_found = fabs(coords[i].lon - lon) < 0.05 ? true : false;
				const bool lat_found = fabs(coords[i].lat - lat) < 0.05 ? true : false;
				if (lon_found && lat_found)
				{
					id = i;
					found = true;
				}
				i++;
			}
			neighbor_index++;
		}

		return id;
	}

int ICoordParser::FindLonIndex(const double lon)
{
	double a = (nlons - 1) / (maxlon - minlon);
	double b = - minlon *(nlons - 1) / (maxlon - minlon);

	double lon_d = a*lon + b;
	return static_cast<int>(std::round(lon_d));
}

int ICoordParser::FindLatIndex(const double lat)
{
	double a = (nlats - 1) / (maxlat - minlat);
	double b = -minlat *(nlats - 1) / (maxlat - minlat);

	double lat_d = a*lat + b;
	return static_cast<int>(std::round(lat_d));
}

bool ICoordParser::Neighbor::operator<(Neighbor const& other) const
{
	if (this->lon_scale < other.lon_scale)
		if (this->lat_scale < other.lat_scale)
			return true;
	return false;
}

std::vector<ICoordParser::Neighbor> ICoordParser::GetNeighbors(double index_scaled_lon, double index_scaled_lat) const
{
	//Check if we are excatly in the middle between two lons and/or two lats
	//if yes we need a strategy how to handle such cases
	//We have up to four possible new candidates	
	//WARNING: Stupid RSharper suggests to remove redundant std:: before abs, this however leads 
	//to a very mean bug: abs without std:: uses the C-abs function from stdlib.h or math.h... 
	//and this function only works for integer/long leading to incorrect results
	std::vector<Neighbor> possible_neighbors;
	if (fabs(fabs(index_scaled_lon) - fabs(floor(index_scaled_lon)) - 0.5 )< RES_ERROR)
	{
		//Four equal neighbors lon and lat have 0.5 distance to neighbors each
		if (fabs(fabs(index_scaled_lat) - fabs(floor(index_scaled_lat)) - 0.5 )< RES_ERROR)
		{
			possible_neighbors.push_back(Neighbor(index_scaled_lon - 0.5, index_scaled_lat - 0.5));
			possible_neighbors.push_back(Neighbor(index_scaled_lon + 0.5, index_scaled_lat - 0.5));
			possible_neighbors.push_back(Neighbor(index_scaled_lon - 0.5, index_scaled_lat + 0.5));
			possible_neighbors.push_back(Neighbor(index_scaled_lon + 0.5, index_scaled_lat + 0.5));
		}

		else
		{
			//lat coord is close enough to an exclusive neighbor
			//only two lons here
			possible_neighbors.push_back(Neighbor(index_scaled_lon - 0.5, round(index_scaled_lat)));
			possible_neighbors.push_back(Neighbor(index_scaled_lon + 0.5, round(index_scaled_lat)));
		}
	}

	//Lon is close enough to an exclusive neighbor but not lat.
	//again two options
	else if (fabs(fabs(index_scaled_lat) - fabs(floor(index_scaled_lat)) - 0.5) < 1E-10)
	{
		possible_neighbors.push_back(Neighbor(round(index_scaled_lon), index_scaled_lat - 0.5));
		possible_neighbors.push_back(Neighbor(round(index_scaled_lon), index_scaled_lat + 0.5));
	}

	//Coords are close enough to an coord(lon, lat)  we have only one suitable candidate
	else
	{
		possible_neighbors.push_back(Neighbor(std::round(index_scaled_lon), std::round(index_scaled_lat)));
	}

	return possible_neighbors;
}

