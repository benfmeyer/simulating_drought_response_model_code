#pragma once
#include <vector>
#include "TransformerType.h"

using std::vector;

template<typename T>
class Transformer2D
{
public:
	Transformer2D(const vector<size_t>& ranks_from, const vector<size_t>& ranks_to, const TransformerType type);
	~Transformer2D();
	vector<T> Convert(const std::vector<T>& data);

private:
	vector<size_t> GetIDs(const vector<size_t>& ranks) const;
	size_t GetID(const size_t& id, const vector<size_t>& ranks) const;
	void InitIDs();
	void Check();


	TransformerType type;
	vector<size_t> ranks_from;
	vector<size_t> ranks_to;
	size_t length;

	vector<size_t> indices_f;
	vector<size_t> indices_t;


	struct Coord2D
	{
		Coord2D() :x(0), y(0)
		{}
		Coord2D(const int x, const int y) :x(x), y(y)
		{}
		int x;
		int y;
	};

	size_t GetID(Coord2D coord, const vector<size_t>& ranks) const;
	Coord2D GetCoord(const size_t& id, const vector<size_t>& ranks) const;
	Coord2D GetCoord_rev(const size_t& id, const vector<size_t>& ranks) const;
};

template <typename T>
Transformer2D<T>::Transformer2D(const vector<size_t>& ranks_from, const vector<size_t>& ranks_to,
	const TransformerType type) :type(type), ranks_from(ranks_from), ranks_to(ranks_to), length(0)
{
	Check();

	InitIDs();
}

template <typename T>
Transformer2D<T>::~Transformer2D()
{
}

template <typename T>
vector<T> Transformer2D<T>::Convert(const std::vector<T>& data)
{
	if (data.size() != length)
		throw("Data and matrix do not match!");

	vector<T> temp_data(length);
	switch (type)
	{
	case TransformerType::LoadAll:
		for (size_t i = 0; i < indices_f.size(); ++i)
			temp_data[indices_t[i]] = data[indices_f[i]];
		break;
	case TransformerType::CalucateWhenNeeded:
		for (int i = 0; i < length; ++i)
		{
			size_t i_f = GetID(i, ranks_from);
			size_t i_t = GetID(i, ranks_to);
			temp_data[i_t] = data[i_f];
		}
		break;
	default:;
	}
	return temp_data;
}

template <typename T>
vector<size_t> Transformer2D<T>::GetIDs(const vector<size_t>& ranks) const
{
	vector<size_t> indices(length);

	size_t i = 0;
	for (size_t x = 0; x < ranks[0]; ++x)
	{
		for (size_t y = 0; y < ranks[1]; ++y)
		{
			const Coord2D c(x, y);
			indices[i] = GetID(c, ranks);
			i++;
		}
	}
	return indices;
}

template <typename T>
size_t Transformer2D<T>::GetID(const size_t& id, const vector<size_t>& ranks) const
{
	return GetID(GetCoord_rev(id, ranks), ranks);
}

template <typename T>
void Transformer2D<T>::InitIDs()
{
	switch (type)
	{
	case TransformerType::LoadAll:
		indices_f = GetIDs(ranks_from);
		indices_t = GetIDs(ranks_to);
		break;
	case TransformerType::CalucateWhenNeeded:
		break;
	default:;
	}
}

template <typename T>
void Transformer2D<T>::Check()
{
	if (ranks_from.size() != ranks_to.size())
		throw("Dimension are unequal");

	size_t length_from = 1;
	for (size_t rank : ranks_from)
		length_from *= rank;

	size_t length_to = 1;
	for (size_t rank : ranks_to)
		length_to *= rank;

	if (length_from != length_to)
		throw("Dimension size does not fit");

	length = length_from;
}

template <typename T>
size_t Transformer2D<T>::GetID(Coord2D coord, const vector<size_t>& ranks) const
{
	return coord.y * ranks[0] + coord.x;
}

template <typename T>
typename Transformer2D<T>::Coord2D Transformer2D<T>::GetCoord(const size_t& id, const vector<size_t>& ranks) const
{
	Coord2D coord;
	coord.y = id / ranks[0];
	coord.x = id % ranks[0];
	return coord;
}

template <typename T>
typename Transformer2D<T>::Coord2D  Transformer2D<T>::GetCoord_rev(const size_t& id, const vector<size_t>& ranks) const
{
	Coord2D coord;
	coord.y = id / ranks[1];
	coord.x = id % ranks[1];
	return coord;
}


