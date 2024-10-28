#pragma once
#ifdef HAVE_MPI
#include <vector>
#include <mpi.h>


template<typename T>
class BufferNCP
{
public:
	BufferNCP(const size_t n_reps, const size_t n_vars, const std::vector<long>& buf_sizes);
	BufferNCP();
	~BufferNCP();

	size_t GetNVars() const;
	size_t GetSize() const;
	const std::vector<T>& GetDataBuffer(size_t index);
	void SetDataBuffer(const std::vector<T>& data, size_t index);
	MPI_Offset GetBBuffer() const;

private:
	MPI_Offset bbufsize_;
	std::vector<std::vector<T>> buffer_;
	size_t n_vars_;
	size_t n_reps_;
	size_t size_;
	void Init(const std::vector<long>& buf_sizes);
};

template <typename T>
BufferNCP<T>::BufferNCP(const size_t n_reps, const size_t n_vars, const std::vector<long>& buf_sizes): bbufsize_(0), n_vars_(0)
{
	std::string error = "Invaild buffer template type";
	throw(error);
}

template <>
inline BufferNCP<float>::BufferNCP(const size_t n_reps, const size_t n_vars, const std::vector<long>& buf_sizes):
	bbufsize_(0), n_vars_(n_vars), n_reps_(n_reps)
{
	Init(buf_sizes);
}

/**
 * \brief Default constructor
 */
template <typename T>
BufferNCP<T>::BufferNCP(): bbufsize_(0), n_vars_(0), n_reps_(0), size_(0)
{
}

template <>
inline BufferNCP<int>::BufferNCP(const size_t n_reps, const size_t n_vars, const std::vector<long>& buf_sizes) :
	bbufsize_(0), n_vars_(n_vars), n_reps_(n_reps)
{
	Init(buf_sizes);
}
template <>
inline BufferNCP<double>::BufferNCP(const size_t n_reps, const size_t n_vars, const std::vector<long>& buf_sizes) :
	bbufsize_(0), n_vars_(n_vars), n_reps_(n_reps)
{
	Init(buf_sizes);
}

template <typename T>
BufferNCP<T>::~BufferNCP()
{
}

template <typename T>
size_t BufferNCP<T>::GetNVars() const
{
	return  n_vars_;
}

template <typename T>
size_t BufferNCP<T>::GetSize() const
{
	return size_;
}

template <typename T>
const std::vector<T>& BufferNCP<T>::GetDataBuffer(const size_t index)
{
	return buffer_[index];
}

template <typename T>
void BufferNCP<T>::SetDataBuffer(const std::vector<T>& data, const size_t index)
{
	buffer_[index] = data;
}

template <typename T>
MPI_Offset BufferNCP<T>::GetBBuffer() const
{
	return  bbufsize_;
}

template <typename T>
void BufferNCP<T>::Init(const std::vector<long>& buf_sizes)
{
	size_ = n_reps_*n_vars_;
	for (int i = 0; i < n_reps_; ++i)
	{
		for (int j = 0; j < n_vars_; ++j)
		{
			std::vector<T> vals_per_var;
			vals_per_var.resize(buf_sizes[j]);
			buffer_.push_back(vals_per_var);
			bbufsize_ += buf_sizes[j] * sizeof(T);
		}
	}
}

#endif
