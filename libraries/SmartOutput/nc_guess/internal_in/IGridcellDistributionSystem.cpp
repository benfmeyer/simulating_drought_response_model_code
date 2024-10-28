#include "IGridcellDistributionSystem.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

IGridcellDistributionSystem::IGridcellDistributionSystem(int n_nodes):n_nodes(n_nodes)
{
}

IGridcellDistributionSystem::~IGridcellDistributionSystem()
{
}

int IGridcellDistributionSystem::GetMaximumGridcellsPerNode() const
{
	return gridcells_per_process;
}

DeckOfCardShuffeling::DeckOfCardShuffeling(int n_nodes): IGridcellDistributionSystem(n_nodes)
{

}


std::vector<int> DeckOfCardShuffeling::DistributeAsList(const std::vector<double>& input_data)
{
	// First get all the number of gridcells per Process. Most likely there might be processes which have more 
	// gridcells then others, however mpi_scatter only works with rectangular arrays.
	const size_t n_gridcells = input_data.size();

	//We increase the number of gridcells_per_process_, so that we have a rectangular array in any case.
	 gridcells_per_process = n_gridcells / n_nodes + 1;

	const int n_super_gridcells = gridcells_per_process * n_nodes;	

	std::vector<int> shuffled_indices(n_super_gridcells);

	//Initialise with invalid cell number
	for (int& index : shuffled_indices)
		index = INVALID_GRICELL;

	//Append shuffle algorithm to the gridcells
	int c_i = 0;
	for (int p = 0; p < n_nodes; ++p)
	{
		for (int c_p = 0; c_p < gridcells_per_process; ++c_p)
		{
			const int index = n_nodes * c_p + p;
			if (index < n_gridcells)
			{
				shuffled_indices[c_i] = index;
			}
			c_i++;
		}
	}


	return  shuffled_indices;
}



bool CompareByValue(const Point& a, const Point& b)
{
	return a.value < b.value;
}

bool MatchByIndex(const Point& a, const Point& b)
{
	return a.index == b.index;
}


AdaptiveGreedy::AdaptiveGreedy(const int n_nodes) : IGridcellDistributionSystem(n_nodes), mean(0.0),
                                                                      total(0.0)
{
}

std::vector<int> AdaptiveGreedy::DistributeAsList(const std::vector<double>& input_data)
{

	// Call the distribution function with input points, so this data will be refactored
	Distribute(input_data);

	// Find out which of the nodes has the highest numbers of indexes/gridcells
	// We need this value to define the y-dimension of the hierachial data
	// that covers all the values.
	CountMaximumNuberOfNodes();

	return RefactorNodesToList();
}

void AdaptiveGreedy::Distribute(const std::vector<Point>& input_data)
{
	// Check if we have negative load values. A negative runtime
	// is not allowed per gridcell
	CheckInputData(input_data);

	// Copy data
	data = input_data;

	//Sort the data in Descending manner
	std::sort(data.rbegin(), data.rend(), CompareByValue);

	// Copy sorted data to be modifyable
	running_data = data;

	// Calculated total load and mean load
	total = 0.0;
	for (Point& p : data)
		total += p.value;
	mean = total / n_nodes;

	// Initialise nodes list with fixed length of nodes
	nodes.resize(n_nodes);

	// Identify last node
	Node& last_Node = nodes.back();
	last_Node.is_last_node = true;
}

void AdaptiveGreedy::Distribute(const std::vector<double>& input_data)
{
	// This function is to transform <double> data to vector of Points
	// with indexes 0,1,2,3,4,5,...

	// Initialise lenght with number of data
	std::vector<Point> input_points(input_data.size());

	//Pass value and index with 0,1,2,3..-
	for (size_t i = 0; i < input_data.size(); i++)
	{
		input_points[i].index = i;
		input_points[i].value = input_data[i];
	}

	//Todo rename that missleading name
	Distribute(input_points);


	// The main algorithim
	// 1. Find indexes and load for the node
	// 2. Delete found entrys of the node from the running list
	for (Node& node : nodes)
	{
		DistributePerNode(node);
		DeleteIndices(node);
	}

}

std::vector<int> AdaptiveGreedy::RefactorNodesToList()
{	
	// This complete functions transforms the highly unhierachal data
	// to hierachical data by adding Invalid Cells to those nodes 
	// who have less indexes compared to others
	
	std::vector<int> indexes_list;

	if (gridcells_per_process*n_nodes > indexes_list.max_size())
		throw GricellDistributionException("Fatal: gridcell_pre_process * n_nodes exceed max interger size");
	
	// Resize array to cover all nodes and gridcells per node
	indexes_list.resize(gridcells_per_process*n_nodes);

	// Initialise all gridcells with an invalid cell number
	for (int& index : indexes_list)
		index = IGridcellDistributionSystem::INVALID_GRICELL;

	// Insert the indexes per list
	for (int i = 0; i < n_nodes; ++i)
	{
		const int delta_t = i*gridcells_per_process;
		const Node& node = nodes[i];

		for (int j = 0; j < node.indicies.size(); ++j)
			indexes_list[delta_t + j] = node.indicies[j];
	}

	return indexes_list;
}

void AdaptiveGreedy::CountMaximumNuberOfNodes()
{
	gridcells_per_process = 0;

	for (Node& node : nodes)
	{
		if (gridcells_per_process < node.indicies.size())
			gridcells_per_process = node.indicies.size();
	}
}

void AdaptiveGreedy::DistributePerNode(Node& node)
{
	// Special treatment for last node:
	// Eat all the remaing gridcells of the running data
	if (node.is_last_node)
	{
		for (const Point& p : running_data)
		{
			node.indicies.push_back(p.index);
			node.load += p.value;
		}
	}

	// Normal distribution for all node expect last
	else
	{
		bool error_descending = true;
		int data_points_left = running_data.size();
		int i = 0;

		while (data_points_left > 0 && error_descending && i < data_points_left)
		{
			int last_index = i;

			// The load of the node is less than the mean value
			// Add up the next index of the running data
			// As the running data is sorted in decending order, this will be 
			// the highest value available
			if (node.load < mean)
			{
				Point p = running_data[i];
				node.indicies.push_back(p.index);
				node.load += p.value;
			}


			// The current load of the node exceeds the mean
			// Take the last value out of the list and search for the best candidate
			// to be as close to the mean as possible.
			if (node.load > mean && node.indicies.size() > 1)
			{
				// Take out the last value
				Point p = running_data[i];
				node.indicies.pop_back();
				node.load -= p.value;

				double load = node.load;

				double oldErr;
				double newErr;

				for (size_t j = last_index; j < data_points_left; j++)
				{
					// Look for the next point
					Point pn = running_data[j];
					// Compute error of the new point added to the load 
					// minus the mean.
					newErr = fabs(load + pn.value - mean);


					// Initialise old error
					if (j == last_index)
						oldErr = newErr + 1.0;

					// As the list is ordered the size of the errors should 
					// be positive and decreasing when processing through the list
					// At one point this flips when we found the best value
					// This simply translates into a new error that is greater than
					// the old one
					// Stop here, we have the best candidate, abort loop
					if (oldErr < newErr)
					{
						// Best candidate found
						// add to list
						Point pn2 = running_data[j - 1];
						node.indicies.push_back(pn2.index);
						node.load += pn2.value;
						
						//Abort loop
						error_descending = false;
						break;
					}

					// If we reach the end of the running data
					// Also aboard loop and take the last value
					// Again, as this is a sorted list (running_data)
					// this value is the smallest possible error at this stage
					if (j == data_points_left - 1)
					{
	
						// add to list
						Point pn2 = running_data[j];
						node.indicies.push_back(pn2.index);
						node.load += pn2.value;

						error_descending = false;
						break;
					}

					//Updated errro
					oldErr = newErr;
				}

			}

			i++;

		}
	}

}

void AdaptiveGreedy::DeleteIndices(Node& node)
{
	for (size_t i = 0; i < node.indicies.size(); i++)
	{
		int position = FindPosition(node.indicies[i]);

		if (position != running_data.size())
			running_data.erase(running_data.begin() + position);
	}
}

void AdaptiveGreedy::Show()
{
	std::cout << "Total is: " << total << " mean is: " << mean << std::endl;

	for (size_t i = 0; i < nodes.size(); i++)
	{
		Node& node = nodes[i];

		std::cout << "Node " << i << " has ids: " << node.indicies.size() << " --- ";

		for (int j = 0; j < node.indicies.size(); ++j)
			std::cout << node.indicies[j] << " ";

		std::cout << " with total =  " << node.load << std::endl;

	}
}

int AdaptiveGreedy::FindPosition(int index)
{
	int i = 0;
	bool found = false;
	while (!found)
	{
		if (index == running_data[i].index)
			found = true;

		i++;
	}
	return i - 1;
}

void AdaptiveGreedy::CheckInputData(const std::vector<Point>& input_data) const
{
	for (const Point& p : input_data)
		if (p.value < 0.0)
		{
			std::string error = "Invalid (negative) load value";
			throw GricellDistributionException(error);
		}
}


