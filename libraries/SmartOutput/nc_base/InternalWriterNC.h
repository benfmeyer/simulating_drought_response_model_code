#pragma once
#include "BaseFileNC.h"
#include "Range.h"
#include <iostream>
#include "Writer.h"


namespace nc_internal
{
	class InternalWriterNC : virtual public BaseFileNC, public Writer
	{
	protected:
		//Empty writer
		InternalWriterNC();
		InternalWriterNC(string filename);
		~InternalWriterNC();

	public:
		void Reopen(string filename, Mode mode) override;
		void SetDimensions(vector<Dimension>& dimensions) override;
		void SetMainGroup(Group& main_group) override;
		void SetGroups(vector<Group>& groups) override;
		void AddGroup(Group& group) override;
		void EndDef() override;

		template <typename  T>
		void WriteData(Variable& variable, const Range& range, const vector<T>& data);
		template <typename  T>
		void WriteData(Variable& variable, const Range& range, const T data);
		template <typename  T>
		void WriteDataNC3(Variable& variable, const Range& range, const vector<T>& data);

	protected:
		void DefineDimensions();
		void DefineGroupsVariablesAttributes(Group& group);
		void AddGroupToList(Group& group);
		void AddDimension(Dimension& dim);
		void AddVariable(Group& group, Variable& variable);

		void AddAttribute(Group& group, Variable& variable, Attribute& attribute);
		void AddGlobalAttribute(Group& group, Attribute& attribute);

		void WriteDataInternal(Variable& variable, const Range& range, const vector<double>& data);
		void WriteDataInternal(Variable& variable, const Range& range, const vector<float>& data);
		void WriteDataInternal(Variable& variable, const Range& range, const vector<int>& data);
		void WriteDataInternal(Variable& variable, const Range& range, const vector<string>& data);

		void WriteDataInternal(Variable& variable, const Range& range, const double data_point);
		void WriteDataInternal(Variable& variable, const Range& range, const float data_point);
		void WriteDataInternal(Variable& variable, const Range& range, const int data_point);
		void WriteDataInternal(Variable& variable, const Range& range, const size_t data_point);
		void WriteDataInternal(Variable& variable, const Range& range, const string data_point);

		//Todo improve the NC3 section
		void WriteDataInternalNC3(Variable& variable, const Range& range, const vector<float> data);
		void WriteDataInternalNC3(Variable& variable, const Range& range, const vector<string> data);
	};

	template <typename T>
	void InternalWriterNC::WriteData(Variable& variable, const Range& range, const vector<T>& data)
	{
		std::cout << "Invalid or not supported type for data writing." << std::endl;
		throw;
	}

	template <typename T>
	void InternalWriterNC::WriteData(Variable& variable, const Range& range, const T data)
	{
		std::cout << "Invalid or not supported type for data writing." << std::endl;
		throw;
	}

	template <typename T>
	void InternalWriterNC::WriteDataNC3(Variable& variable, const Range& range, const vector<T>& data)
	{
		std::cout << "Invalid or not supported type for data writing." << std::endl;
		throw;
	}

	template<>
	inline void InternalWriterNC::WriteData<double>(Variable& variable, const Range& range, const vector<double>& data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<float>(Variable& variable, const Range& range, const vector<float>& data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<int>(Variable& variable, const Range& range, const vector<int>& data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<string>(Variable& variable, const Range& range, const vector<string>& data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteDataNC3<float>(Variable& variable, const Range& range, const vector<float>& data)
	{
		WriteDataInternalNC3(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<double>(Variable& variable, const Range& range, const double data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<float>(Variable& variable, const Range& range, const float data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<int>(Variable& variable, const Range& range, const int data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<size_t>(Variable& variable, const Range& range, const size_t data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterNC::WriteData<string>(Variable& variable, const Range& range, const string data)
	{
		WriteDataInternal(variable, range, data);
	}

}

