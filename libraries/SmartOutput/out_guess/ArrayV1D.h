#pragma once
#include "ArrayV.h"

template<typename T>
class ArrayV1D : public ArrayV<T>
{
public:
	ArrayV1D();
	ArrayV1D(size_t size);	
	void SetPart(size_t offset, const std::vector<T>& part);
	void SetPart(size_t offset, const ArrayV1D<T>& part);

	~ArrayV1D();
};

template <typename T>
ArrayV1D<T>::ArrayV1D():ArrayV<T>::ArrayV()
{
	//Nothing to do here
}

template <typename T>
ArrayV1D<T>::ArrayV1D(size_t size)
{
	if (this->data_.max_size() < size)
	{
		std::string err = "Vector overflow";
		throw(err);
	}

	this->total_size_ = size;
	this->data_.resize(this->total_size_);
}

template <typename T>
void ArrayV1D<T>::SetPart(const size_t offset, const std::vector<T>& part)
{
	if (offset + part.size() > this->total_size_)
		throw("ArrayV1D: sum of offset and part is greater than total size of array");

	for (int i = 0; i < part.size(); ++i)
	{
		this->data_[offset + i] = part[i];
	}
}

template <typename T>
void ArrayV1D<T>::SetPart(size_t offset, const ArrayV1D<T>& part)
{
	SetPart(offset, part.GetAllValues());
}

template <typename T>
ArrayV1D<T>::~ArrayV1D()
{

}
