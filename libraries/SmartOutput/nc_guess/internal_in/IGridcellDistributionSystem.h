//////////////////////////////////////////////////////////////////////////////////////
/// \file IGridcellDistributionSystem.h
/// \brief Inteface for different gridcell distribution systems across gridcells using
/// deck of card shuffling and adaptive greedy.
/// 
/// \author Phillip Papastefanou
/// $Date: 2018-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>
#include <limits>
#include <cstddef>
#include <stdexcept>
#include <string>


class IGridcellDistributionSystem
{
public:
	IGridcellDistributionSystem(int n_nodes);
	virtual ~IGridcellDistributionSystem();

	const static int INVALID_GRICELL = std::numeric_limits<int>::max();

	virtual std::vector<int> DistributeAsList(const std::vector<double>& input_data) = 0;
	int GetMaximumGridcellsPerNode() const;
protected:
	int n_nodes;
	int gridcells_per_process;

};

class DeckOfCardShuffeling : public IGridcellDistributionSystem
{
public:
	DeckOfCardShuffeling(int n_nodes);
	std::vector<int> DistributeAsList(const std::vector<double>& input_data) override;

};

struct Point
{
	int index;
	double value;
};

struct Node
{
	Node() :load(0.0), is_last_node(false) { }

	std::vector<int> indicies;
	double load;
	bool is_last_node;
};


/**
 * \brief This algorithm distributions gridcells across nodes following an adaptive
 * greedy sheme, also know as the k-subset problem. This problem is NP hard hence there is no
 * elementary solution available
 * This solution of this problem (been an adaption of the greedy algorithm) has
 * been tested under various conditions: It works perfectly with very heterogenous data,
 * e.g. uniform distributed load and very good with normal distributed load. 
 * If the average load per gridcell is almost constant
 * this algorithm is not the best choice; in this case take DeckOfCardShuffeling as
 * alternative. The procedure can be described as following: Foreach node the expected load is filled
 * starting with the larges values of load per gridcell. Once the load per node exceeds the mean
 * load per gridcell the last value is taken out and the best candidate of the remaining gridcells
 * is beeing searched for. Subsequently, the gridcell indexes are taken out of the list and the next node
 * repeats this procedure with the remaining values. The last node simply will get all the remaining gridcells 
 * in the gridcell list.
 */
class AdaptiveGreedy : public IGridcellDistributionSystem
{
public:
	/// Initialise with number of nodes
	AdaptiveGreedy(int n_nodes);

	/// Get the nodes as hierachical list, because we need that for MPI_SCATTER
	// NA values are omitted via the InvalidGridcell value
	std::vector<int> DistributeAsList(const std::vector<double>& input_data) override;

private:
	void Distribute(const std::vector<Point>& input_data);
	void Distribute(const std::vector<double>& input_data);
	std::vector<int> RefactorNodesToList();
	void CountMaximumNuberOfNodes();

	/// The adaptive greedy distribution function
	void DistributePerNode(Node& node);

	void DeleteIndices(Node& node);
	void Show();
	int FindPosition(int index);
	void CheckInputData(const std::vector<Point>& input_data) const;

	std::vector<Point> data;
	std::vector<Point> running_data;

	std::vector<int> indicies;
	double mean;
	double total;


	std::vector<Node> nodes;

};

class GricellDistributionException : public std::runtime_error
{
public:
	GricellDistributionException(const std::string what) : runtime_error(what) {}
};

