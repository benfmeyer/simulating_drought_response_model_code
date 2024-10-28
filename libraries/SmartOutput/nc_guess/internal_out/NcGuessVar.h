#pragma once
#include "NcGuessVarBase.h"
#include "OutTypes.h"

using std::string;

	template<typename T>
	class NcGuessVar : public NcGuessVarBase<T>
	{
	public:
		//OutVariable(const OutVariable&):OutVariableBase("", NC_CHAR) {}
		NcGuessVar & operator=(NcGuessVar& a) { return *this; }
		NcGuessVar(string name);
		~NcGuessVar();
	};

	template <typename T>
	NcGuessVar<T>::NcGuessVar(string name) : NcGuessVarBase<T>(nc_internal::DataType::TEXT)
	{
		fail("Invalid nc variable type");
	}

	template <typename T>
	NcGuessVar<T>::~NcGuessVar()
	{
	}


	template<>
	class NcGuessVar<float> : public NcGuessVarBase<float>
	{
	public:
		NcGuessVar & operator=(NcGuessVar& a) { return *this; }

		NcGuessVar(out_guess::OutputVariable& var, BaseWriter& writer, Group& group, const size_t n_years, const OutWriterType writer_type, int stand_id = 0) :
			NcGuessVarBase(var, nc_internal::DataType::FLOAT, writer, group, n_years, writer_type, stand_id)
		{
		}

		//void AddValues()
		//{
		//	switch (writer_type_)
		//	{
		//		case OutWriterType::Daily:
		//			data_.push_back(out_var_.GetStandDataDay());
		//		break;		
		//		case OutWriterType::Monthly:
		//			data_.push_back(out_var_.GetStandDataMonth());
		//		break;		
		//		case OutWriterType::Annualy: 
		//			data_.push_back(out_var_.GetStandDataYear());
		//		break;			
		//		case OutWriterType::Invalid:
		//		break;
		//	default: ;
		//	}		
		//}

		~NcGuessVar()
		{

		}
	};

	template<>
	class NcGuessVar<double> : public NcGuessVarBase<double>
	{
	public:
		NcGuessVar & operator=(NcGuessVar& a) { return *this; }

		/**
		 * \brief Most common template specification of NcGuessVar: double. Creates a Netcdf-Variable according to the Output variables
		 * \param var Output variable reference
		 * \param writer Guess-NetCDF-writer reference
		 * \param group Group name of the NetCDF variable
		 * \param writer_type Should be daily, monthly or annualy
		 * \param downcasting If this selected values with double prec will be saved as floats using less space
		 */
		NcGuessVar(out_guess::OutputVariable& var, BaseWriter& writer, Group& group, const OutWriterType writer_type, const size_t n_years,  bool downcasting = false, int stand_id = 0) :
			NcGuessVarBase(var, downcasting ? nc_internal::DataType::FLOAT : nc_internal::DataType::DOUBLE, writer, group, n_years, writer_type, stand_id)
		{
		}
		void AddValues()
		{
			switch (writer_type_)
			{
			case OutWriterType::Daily:
			{	
				ArrayV2D<double>* tau = out_var_.GetStandDay2D();
				data_2D_->SetSubArray(year_index_ * 365, 0, *tau);
			}
			break;

			case OutWriterType::Monthly:
			{
				ArrayV2D<double>* tau = out_var_.GetStandMonth2D();
				data_2D_->SetSubArray(year_index_ * 12, 0, *tau);
			}
			break;

			case OutWriterType::Annualy:
			{				
				ArrayV2D<double>* tau = out_var_.GetStandYear2D();
				data_2D_->SetSubArray(year_index_, 0, *tau);
			}
			break;

			case OutWriterType::Invalid:
				break;
			default:;
			}

			year_index_++;
		}


		~NcGuessVar()
		{

		}

	};

	template<>
	class NcGuessVar<int> : public NcGuessVarBase<int>
	{
	public:
		//OutVariable(const OutVariable&) :OutVariableBase("", NC_FLOAT, PftDetailLevel::Invalid) {}
		NcGuessVar & operator=(const NcGuessVar& a) { return *this; }

		NcGuessVar(out_guess::OutputVariable& var, BaseWriter& writer, Group& group, const OutWriterType writer_type, const size_t n_years, bool downcasting = false, int stand_id = 0) 
		: NcGuessVarBase(var, nc_internal::DataType::INT, writer, group, n_years, writer_type, stand_id)
		{

		}

		~NcGuessVar()
		{

		}
	};


