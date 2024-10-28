#pragma once
#include <vector>
#include "TransformerType.h"

using std::vector;

template<typename T>
class Transformer4D
{
public:
	Transformer4D(const vector<size_t>& ranks_from, const vector<size_t>& ranks_to, const TransformerType type);
	~Transformer4D();
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


	struct Coord4D
	{
		Coord4D() :a(0), b(0), c(0), d(0)
		{}
		Coord4D(const int a, const int b, const int c, const int d) :a(a), b(b), c(c), d(d)
		{}
		int a;
		int b;
		int c;
		int d;
	};

	size_t GetID(Coord4D coord, const vector<size_t>& ranks) const;
	Coord4D GetCoord(const size_t& id, const vector<size_t>& ranks) const;
	Coord4D GetCoord_rev(const size_t& id, const vector<size_t>& ranks) const;
};

template <typename T>
Transformer4D<T>::Transformer4D(const vector<size_t>& ranks_from, const vector<size_t>& ranks_to,
	const TransformerType type) :type(type), ranks_from(ranks_from), ranks_to(ranks_to), length(0)
{
	Check();

	InitIDs();
}

template <typename T>
Transformer4D<T>::~Transformer4D()
{
}

template <typename T>
vector<T> Transformer4D<T>::Convert(const std::vector<T>& data)
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
vector<size_t> Transformer4D<T>::GetIDs(const vector<size_t>& ranks) const
{
	vector<size_t> indices(length);

	size_t i = 0;
	for (size_t a = 0; a < ranks[0]; ++a)
	{
		for (size_t b = 0; b < ranks[1]; ++b)
		{
			for (size_t c = 0; c < ranks[2]; ++c)
			{
				for (size_t d = 0; d < ranks[3]; ++d)
				{
					const Coord4D co(a, b, c, d);
					indices[i] = GetID(co, ranks);
					i++;
				}
			}
		}
	}
	return indices;
}

template <typename T>
size_t Transformer4D<T>::GetID(const size_t& id, const vector<size_t>& ranks) const
{
	return GetID(GetCoord_rev(id, ranks), ranks);
}

template <typename T>
void Transformer4D<T>::InitIDs()
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
void Transformer4D<T>::Check()
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
size_t Transformer4D<T>::GetID(Coord4D coord, const vector<size_t>& ranks) const
{
	return coord.d * ranks[0] * ranks[1] * ranks[2] + coord.c * ranks[0] * ranks[1] + coord.b * ranks[0] + coord.a;
}

template <typename T>
typename Transformer4D<T>::Coord4D Transformer4D<T>::GetCoord(const size_t& id, const vector<size_t>& ranks) const
{
	Coord4D coord;
	coord.d = id / (ranks[0] * ranks[1] * ranks[2]);
	const long long t1 = id - coord.d * ranks[0] * ranks[1] * ranks[2];
	coord.c =  t1 / (ranks[0] * ranks[1]);
	const long long t2 = t1 - coord.c * ranks[0] * ranks[1];
	coord.b= t2 / ranks[0];
	coord.a = t2 % ranks[0];
	return coord;
}

template <typename T>
typename Transformer4D<T>::Coord4D Transformer4D<T>::GetCoord_rev(const size_t& id, const vector<size_t>& ranks) const
{
	Coord4D coord;
	coord.a = id / (ranks[1] * ranks[2] * ranks[3]);
	const long long t1 = id - coord.a * ranks[1] * ranks[2] * ranks[3];
	coord.b = t1 / (ranks[2] * ranks[3]);
	const long long t2 = t1 - coord.b * ranks[2] * ranks[3];
	coord.c = t2 / ranks[3];
	coord.d = t2 % ranks[3];
	return coord;
}

