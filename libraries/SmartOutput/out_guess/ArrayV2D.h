#pragma once
#include "ArrayV.h"
#include <functional>

template<typename T>
class ArrayV2D :	public ArrayV<T>
{
public:
	ArrayV2D(size_t x_size, size_t y_size);
	ArrayV2D();
	~ArrayV2D();

	/**
	 * \brief Sets value at position(x,y)
	 * \param x index x
	 * \param y index y
	 * \param value value to be set in the array
	 */
	void Set(size_t x, size_t y, T value);	

	void SetSubArray(size_t offset_x, size_t offset_y, ArrayV2D<T> sub_data);
	void SetCol(size_t x, const std::vector<T>& values);
	void SetRow(size_t y, const std::vector<T>& values);

	/**
	 * \brief Accumulates value at position(x,y)
	 * \param x index x
	 * \param y index y
	 * \param value value to be accumulated in the array
	 */
	void Acc(size_t x, size_t y, T value);
	void Acc(size_t i, T value) override;

	ArrayV2D Transpose();

	/**
	 * \brief Gets value of array at position(x,y)
	 * \param x index x
	 * \param y index y
	 * \return 
	 */
	T Get(size_t x, size_t y) const;
	T Get(size_t i) const override;

	/**
	 * \brief Get the number of rows
	 * \return 
	 */
	size_t GetSizeX() const;
	
	/**
	 * \brief Get the number of colums
	 * \return 
	 */
	size_t GetSizeY() const;
	
	/**
	 * \brief Gets the whole row at column position y
	 * \param y column position
	 * \return 
	 */
	std::vector<T> GetXRow(const size_t y);


	/**
	 * \brief Gets the whole column at row position x
	 * \param x row position
	 * \return 
	 */
	std::vector<T> GetYCol(const size_t x);

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

	std::function<void(size_t x, size_t y, T value)> SetI;
};

template <typename T>
ArrayV2D<T>::ArrayV2D(size_t x_size, size_t y_size):x_size_(x_size), y_size_(y_size)
{
	if (this->data_.max_size() < x_size_ * y_size_)
	{
		std::string error = "Vector overflow at ArrayV2D";
		throw error;
	}

	this->total_size_ = x_size * y_size;
	this->data_.resize(x_size_* y_size_);
}

template <typename T>
ArrayV2D<T>::ArrayV2D(): ArrayV<T>::ArrayV(), x_size_(0), y_size_(0)
{
}

template <typename T>
ArrayV2D<T>::~ArrayV2D()
{

}

template <typename T>
void ArrayV2D<T>::Set(size_t x, size_t y, T value)
{
	this->data_[x + x_size_* y] = value;
}

template <typename T>
void ArrayV2D<T>::SetSubArray(size_t offset_x, size_t offset_y, ArrayV2D<T> sub_data)
{
	if (offset_x + sub_data.GetSizeX() > x_size_)
		throw("Offset and subsize are greater of x_size");
	if (offset_y + sub_data.GetSizeY() > y_size_)
		throw("Offset and subsize are greater of y_size");

	for (size_t x = 0; x < sub_data.GetSizeX(); ++x)
	{
		for (size_t y = 0; y < sub_data.GetSizeY(); ++y)
		{
			auto data = sub_data.Get(x, y);
			Set(x + offset_x, y + offset_y, data);
		}
	}
}

template <typename T>
void ArrayV2D<T>::SetCol(size_t x, const std::vector<T>& values)
{
	if (values.size() != y_size_)
		throw("Values mismatch");

	for (size_t y = 0; y < values.size(); ++y)
	{
		this->data_[x + x_size_* y] = values[y];
	}	
}

template <typename T>
void ArrayV2D<T>::SetRow(size_t y, const std::vector<T>& values)
{
	if (values.size() != x_size_)
		throw("Values mismatch");

	for (size_t x = 0; x < values.size(); ++x)
	{
		this->data_[x + x_size_* y] = values[x];
	}
}

template <typename T>
void ArrayV2D<T>::Acc(size_t x, size_t y, T value)
{
	this->data_[x + x_size_* y] += value;
}

template <typename T>
void ArrayV2D<T>::Acc(size_t i, T value)
{
	ArrayV<T>::Acc(i, value);
}

template <typename T>
ArrayV2D<T> ArrayV2D<T>::Transpose()
{
	ArrayV2D newArr(y_size_, x_size_);
	for (size_t y = 0; y < y_size_; ++y)
	{
		// to avoid  error: initial value of reference to non-const must be an lvalue
		const std::vector<T> row = this->GetXRow(y);
		newArr.SetCol(y, row);
	}
	return newArr;
}

template <typename T>
T ArrayV2D<T>::Get(size_t x, size_t y) const
{
	return  this->data_[x + x_size_*y];
}

template <typename T>
T ArrayV2D<T>::Get(size_t i) const
{
	return ArrayV<T>::Get(i);
}

template <typename T>
size_t ArrayV2D<T>::GetSizeX() const
{
	return x_size_;
}

template <typename T>
size_t ArrayV2D<T>::GetSizeY() const
{
	return  y_size_;
}

template <typename T>
std::vector<T> ArrayV2D<T>::GetXRow(const size_t y)
{
	std::vector<T> vec(x_size_);
	for (size_t x = 0; x < x_size_; ++x)
	{
		vec[x] = this->data_[x + x_size_*y];
	}
	return vec;
}

template <typename T>
std::vector<T> ArrayV2D<T>::GetYCol(const size_t x)
{
	std::vector<T> vec(y_size_);
	for (size_t y = 0; y < y_size_; ++y)
	{
		vec[y] = this->data_[x + x_size_*y];
	}
	return vec;
}
