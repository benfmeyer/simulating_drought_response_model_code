#pragma once
#include <vector>
#include <stddef.h>

template<typename T>
class ArrayV
{
public:
	ArrayV();
	virtual ~ArrayV() = default;

	typedef T* iterator;
	typedef const T* const_iterator;

	typename std::vector<T>::iterator begin() { return data_.begin(); }
	typename std::vector<T>::iterator end() { return data_.end(); }

	typename std::vector<T>::const_iterator begin() const { return  data_.begin(); }
	typename std::vector<T>::const_iterator end() const { return  data_.end(); }

	virtual T Get(size_t i) const;
	virtual void Set(size_t i, T value);

	T& operator[] (size_t i);
	T& operator[] (size_t i) const;
	virtual void Acc(size_t i, T value);

	size_t GetSizeTotal() const;
	std::vector<T>& GetAllValues();

protected:
	std::vector<T> data_;
	size_t total_size_;

};

template <typename T> 
ArrayV<T>::ArrayV():total_size_(0)
{

}

template <typename T>
T ArrayV<T>::Get(size_t i) const
{
	return data_[i];
}

template <typename T>
void ArrayV<T>::Set(size_t i, T value)
{
	data_[i] = value;
}

template <typename T>
T& ArrayV<T>::operator[](const size_t i)
{
	return data_[i];

}

template <typename T>
T& ArrayV<T>::operator[](const size_t i) const
{
	return data_[i];
}

template <typename T>
void ArrayV<T>::Acc(size_t i, T value)
{
	data_[i] += value;
}

template <typename T>
size_t ArrayV<T>::GetSizeTotal() const
{
	return total_size_;
}

template <typename T>
std::vector<T>& ArrayV<T>::GetAllValues()
{
	return data_;
}
