#pragma once
#include "ArrayV.h"
#include "ArrayV2D.h"

template<typename T>
class ArrayV3D : public ArrayV<T>
{
public:
	ArrayV3D();
	ArrayV3D(size_t x_size, size_t y_size, size_t z_size);
	/**
	 * \brief Generates a ArrayV3D based of a vector of ArrayV2D. Thereby the vector.size() will be treated
	 * as dimension x, the ArrayV2D dimension x, y as y and z, respectively.
	 * \param data 
	 */
	ArrayV3D(const std::vector<ArrayV2D<T>>& data);

	~ArrayV3D();

	/**
	* \brief Sets value at position(x,y)
	* \param x index x
	* \param y index y
	* \param z index z
	* \param value value to be set in the array
	*/
	void Set(size_t x, size_t y, size_t z, T value);

	/**
	* \brief Accumulates value at position(x,y)
	* \param x index x
	* \param y index y
	* \param z index z
	* \param value value to be accumulated in the array
	*/
	void Acc(size_t x, size_t y, size_t z, T value);

	/**
	* \brief Gets value of array at position(x,y)
	* \param x index x
	* \param y index y
	* \param z index z
	* \return
	*/
	T Get(size_t x, size_t y, size_t z);
	T Get(size_t i) const override;

	/**
	* \brief Get the number of dim x
	* \return
	*/
	size_t GetSizeX() const;

	/**
	* \brief Get the number of dim y
	* \return
	*/
	size_t GetSizeY() const;

	/**
	* \brief Get the number of dim z
	* \return
	*/
	size_t GetSizeZ() const;


	//class Proxy {
	//public:
	//	Proxy(std::vector<T>& array) : array_(array) { }

	//	T operator[](size_t index) {
	//		return array_[index];
	//	}
	//private:
	//	std::vector<T>& array_;
	//};
private:
	size_t x_size_;
	size_t y_size_;
	size_t z_size_;
};

template <typename T>
ArrayV3D<T>::ArrayV3D(size_t x_size, size_t y_size, size_t z_size) :x_size_(x_size), y_size_(y_size), z_size_(z_size)
{
	if (this->data_.max_size() < x_size_ * y_size_* z_size_)
	{
		std::string error = "Vector overflow at ArrayV3D";
		throw error;
	}

	this->total_size_ = x_size_ * y_size_* z_size_;
	this->data_.resize(x_size_ * y_size_* z_size_);
}

template <typename T>
ArrayV3D<T>::ArrayV3D(const std::vector<ArrayV2D<T>>& data): x_size_(0), y_size_(0), z_size_(0)
{
	size_t x_size = data.size();
	size_t y_size = data[0].GetSizeX();
	size_t z_size = data[0].GetSizeY();

	ArrayV3D<T>(x_size, y_size, z_size);

	size_t i = 0;
	const size_t yz_length = y_size * z_size;

	for (const ArrayV2D<T>& arr2d : data)
	{
		for (size_t j = 0; j < yz_length; ++j)
		{
			this->data_[i * yz_length + j] = arr2d.Get(j);
		}
		i++;
	}
}

template <typename T>
ArrayV3D<T>::ArrayV3D() : ArrayV<T>::ArrayV(), x_size_(0), y_size_(0), z_size_(0)
{
}

template <typename T>
ArrayV3D<T>::~ArrayV3D()
{

}

template <typename T>
void ArrayV3D<T>::Set(size_t x, size_t y, size_t z, T value)
{
	this->data_[x + x_size_* y + x_size_*y_size_*z] = value;
}

template <typename T>
void ArrayV3D<T>::Acc(size_t x, size_t y, size_t z, T value)
{
	this->data_[x + x_size_* y + x_size_*y_size_*z] += value;
}

template <typename T>
T ArrayV3D<T>::Get(size_t x, size_t y, size_t z)
{
	return  this->data_[x + x_size_* y + x_size_*y_size_*z];
}

template <typename T>
T ArrayV3D<T>::Get(size_t i) const
{
	return ArrayV<T>::Get(i);
}

template <typename T>
size_t ArrayV3D<T>::GetSizeX() const
{
	return x_size_;
}

template <typename T>
size_t ArrayV3D<T>::GetSizeY() const
{
	return  y_size_;
}

template <typename T>
size_t ArrayV3D<T>::GetSizeZ() const
{
	return z_size_;
}

