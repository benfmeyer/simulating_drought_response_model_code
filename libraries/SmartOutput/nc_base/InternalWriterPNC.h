#pragma once
#ifdef HAVE_PNETCDF
#include "BaseFilePNC.h"
#include "BufferNCP.h"
#include "Writer.h"

namespace nc_internal
{

	class InternalWriterPNC :public virtual BaseFilePNC , public Writer
	{
	protected:
		//Empty writer
		InternalWriterPNC();
		InternalWriterPNC(string filename);
		~InternalWriterPNC();

	public:
		void Reopen(string filename, Mode mode) override;
		void SetDimensions(vector<Dimension>& dimensions) override;
		void SetMainGroup(Group& main_group) override;
		void SetGroups(vector<Group>& groups) override;
		void AddGroup(Group& group) override;

		void EndDef() override;

		//Todo this actually belongs to a PNCReader. However, we currently do not need a reader...
		Group& GetGroup(string name);
		Variable& GetVariable(string grpname, string varname);


		//Collective at the end write
		template <typename  T>
		void WriteData(Variable& variable, const Range& range, const vector<T>& data);

		void StartIndependent(int number_of_total_variabls);

		template <typename  T>
		void WriteDataI(Variable& variable, const Range& range, const vector<T>& data);
		template <typename  T>
		void WriteDataI(Variable& variable, const Range& range, const T& data);

		template <typename  T>
		void WriteDataB(Variable& variable, const Range& range, const vector<T>& data);


		void EndIndependent();


		void SetFloatBuffer(const long n_vars, const long n_reps, const vector<long> buf_sizes);
		void SetIntBuffer(const long n_vars, const long n_reps, const vector<long> buf_sizes);
		void AttachBuffer();
		void FullBufferedEnd();	

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
		//Leave this in for future implementations
		//void WriteDataInternal(Variable& variable, const Range& range, const vector<string>& data);

		void WriteDataInternalIndependend(Variable& variable, const Range& range, const vector<int>& data);
		void WriteDataInternalIndependend(Variable& variable, const Range& range, const vector<float>& data);
		void WriteDataInternalIndependend(Variable& variable, const Range& range, const vector<double>& data);
		void WriteDataInternalIndependend(Variable& variable, const Range& range, const string& data);

		void WriteDataInternalFullBuffered(Variable& variable, const Range& range, const vector<float>& data);
		void WriteDataInternalFullBuffered(Variable& variable, const Range& range, const vector<double>& data);
		void WriteDataInternalFullBuffered(Variable& variable, const Range& range, const vector<int>& data);

		string ReplaceUnderscore(string text) const;
		NcmpiVar& FindVariable(string name);

		vector<NcmpiDim> ncmpi_dims_;
		vector<NcmpiVar> ncmpi_vars_;




		//checks for the first output
		bool firstOutput_;

		//Number of repetitons of bufferd write functions
		int n_reps_;


		//Independed write members
		int n_vars_float_;
		//Independed write members
		int n_vars_int_;
		//Running variable index float
		int var_index_float;
		//Running variable index int
		int var_index_int;
		//Running variable index total;
		int var_index_total;
		//Array of ncmpi_errors
		vector<int> st_;
		//Array of requests
		vector<int> req_;

		MPI_Offset bbufsize_;

		vector<NcmpiVar> buffer_vars_;
		vector<Range> buffer_ranges_;	

		BufferNCP<float> float_buffer_;
		BufferNCP<int> int_buffer_;


		//NcmpiGroup ncmpi_maingroup_;
		//vector<NcmpiGroup> ncmpi_groups_;
	};


	template <typename T>
	void InternalWriterPNC::WriteDataI(Variable& variable, const Range& range, const vector<T>& data)
	{
		//Todo introduce exception handling
		std::cout << "Invalid writing type" << std::endl;
	}
	template<>
	inline void InternalWriterPNC::WriteDataI<float>(Variable& variable, const Range& range, const vector<float>& data)
	{
		WriteDataInternalIndependend(variable, range, data);
	}
	template<>
	inline void InternalWriterPNC::WriteDataI<int>(Variable& variable, const Range& range, const vector<int>& data)
	{
		WriteDataInternalIndependend(variable, range, data);
	}

	template <typename T>
	void InternalWriterPNC::WriteDataI(Variable& variable, const Range& range, const T& data)
	{
		//Todo introduce exception handling
		std::cout << "Invalid Writering type" << std::endl;
	}

	template <typename T>
	void InternalWriterPNC::WriteDataB(Variable& variable, const Range& range, const vector<T>& data)
	{
		//Todo introduce exception handling
		std::cout << "Invalid Writering type" << std::endl;
	}

	template<>
	inline void InternalWriterPNC::WriteDataB<float>(Variable& variable, const Range& range, const vector<float>& data)
	{
		WriteDataInternalFullBuffered(variable, range, data);
	}
	template<>
	inline void InternalWriterPNC::WriteDataB<double>(Variable& variable, const Range& range, const vector<double>& data)
	{
		WriteDataInternalFullBuffered(variable, range, data);
	}
	template<>
	inline void InternalWriterPNC::WriteDataB<int>(Variable& variable, const Range& range, const vector<int>& data)
	{
		WriteDataInternalFullBuffered(variable, range, data);
	}



	template<>
	inline void InternalWriterPNC::WriteDataI<string>(Variable& variable, const Range& range, const string& data)
	{
		WriteDataInternalIndependend(variable, range, data);
	}


	template<>
	inline void InternalWriterPNC::WriteData<double>(Variable& variable, const Range& range, const vector<double>& data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterPNC::WriteData<float>(Variable& variable, const Range& range, const vector<float>& data)
	{
		WriteDataInternal(variable, range, data);
	}
	template<>
	inline void InternalWriterPNC::WriteData<int>(Variable& variable, const Range& range, const vector<int>& data)
	{
		WriteDataInternal(variable, range, data);
	}

}

#endif

