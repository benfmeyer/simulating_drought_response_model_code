#pragma once
#include "Group.h"
#include "Types.h"
#include "ArrayV.h"
#include "OutputVarPatch.h"
#include "OutputVarPft.h"
#include "ArrayV3D.h"
#include "OutTypes.h"
#include <memory>
#include <limits>


class BaseWriter;

	using std::string;
	using namespace nc_base;

	template<typename T>
	class NcGuessVarBase
	{
	public:
		NcGuessVarBase(out_guess::OutputVariable& out_var, const nc_internal::DataType type, BaseWriter& writer, Group& group, size_t n_years, OutWriterType writer_type, int stand_id) :
			writer_(writer),
			out_var_(out_var),
			type_(type),
			group_(group),
			is_patch_var_(false),
			patch_detail_level_(out_guess::PatchDetailLevel::Invalid),
			is_pft_var_(false),
			pft_detail_level_(out_guess::PftDetailLevel::Invalid),
			nyears_(n_years),
			year_index_(0), 
			writer_type_(writer_type),
			stand_id_(stand_id)
		{
			SetUp();
		}

		out_guess::OutputVariable& GetOutVar() const
		{
			return out_var_;
		}
		string GetGroupName() const
		{
			return  group_.name;
		}

		Variable& GetNcVar() const
		{
			return *nc_var_;
		}

		ArrayV2D<T>* GetData2D()
		{
			return data_2D_.get();
		}

		string GetName() const
		{
			return  name_;
		}

		int GetStandID() const
		{
			return  stand_id_;
		}

		void Clear()
		{
			year_index_ = 0;
		}

		~NcGuessVarBase()
		{

		}

		bool IsPatchVar() const;
		out_guess::PatchDetailLevel PatchDetailLevel() const;
		bool IsPftVar() const;
		out_guess::PftDetailLevel PftDetailLevel() const;

	protected:
		size_t nyears_;
		size_t year_index_;
		
		BaseWriter& writer_;
		OutWriterType writer_type_;
		size_t time_steps_per_year_;

		int stand_id_;
		out_guess::OutputVariable& out_var_;
		string name_;
		string unit_;
		nc_internal::DataType type_;
		Group& group_;
		std::shared_ptr<ArrayV2D<T>> data_2D_;
		void SetUp();

	private:
		void AddPatchVar(const out_guess::OutputVarPatch& patch_var);
		void AddPftVar(const out_guess::OutputVarPft& pft_var);

		//This has to be a shared pointer as copying would not be allowed
		std::shared_ptr<Variable> nc_var_;
		//Todo figure out the problem with the dynamic_pointer_casts here
		//std::shared_ptr<out_guess::OutputVarPatch> out_var_patch_;
		//std::shared_ptr<out_guess::OutputVarPft> out_var_pft_;

		bool is_patch_var_;
		out_guess::PatchDetailLevel patch_detail_level_;
		bool is_pft_var_;
		out_guess::PftDetailLevel pft_detail_level_;


	};

	template <typename T>
	void NcGuessVarBase<T>::SetUp()
	{
		unit_ = out_var_.GetUnit();
		name_ = out_var_.GetName();

		switch (writer_type_)
		{
		case OutWriterType::Daily:
			time_steps_per_year_ = 365;
			break;
		case OutWriterType::Monthly:
			time_steps_per_year_ = 12;
			break;
		case OutWriterType::Annualy:
			time_steps_per_year_ = 1;
			break;
		case OutWriterType::Invalid:
			break;
		default:;
		}


		nc_var_ = std::make_unique<Variable>(group_);		
		
		if (unit_.size() > 0)
		{
			nc_var_->attributes.push_back(Attribute("unit", unit_));
		}

		//Push back FillValue attribute
		nc_var_->attributes.push_back(Attribute("_FillValue", std::to_string(std::numeric_limits<T>::infinity()), nc_internal::DataType::FLOAT));

		nc_var_->name = out_var_.GetName();
		nc_var_->type = type_;

		//Todo move this Deflation out of here
		nc_var_->def_level = nc_internal::DeflationLevel::None;
		
		out_guess::OutputVarPft* out_var_pft = dynamic_cast<out_guess::OutputVarPft*>(&out_var_);
		out_guess::OutputVarPatch* out_var_patch = dynamic_cast<out_guess::OutputVarPatch*>(&out_var_);
		//out_var_patch_ = std::dynamic_pointer_cast<out_guess::OutputVarPatch>(std::make_shared<out_guess::OutputVariable>(out_var_));

		if (out_var_patch != nullptr)
		{
			AddPatchVar(*out_var_patch);
			group_.name = "Patch-Out";
		}
		else if (out_var_pft != nullptr)
		{
			AddPftVar(*out_var_pft);
			group_.name = "Pft-Out";
		}

		else
		{
			string error = "Invalid OutputVariable Type";
			throw(error);
		}

		if (stand_id_ == 0)
		{
			group_.variables.push_back(*nc_var_);
		}
	}

	template <typename T>
	bool NcGuessVarBase<T>::IsPatchVar() const
	{
		return is_patch_var_;
	}

	template <typename T>
	out_guess::PatchDetailLevel NcGuessVarBase<T>::PatchDetailLevel() const
	{
		return patch_detail_level_;
	}

	template <typename T>
	bool NcGuessVarBase<T>::IsPftVar() const
	{
		return is_pft_var_;
	}

	template <typename T>
	out_guess::PftDetailLevel NcGuessVarBase<T>::PftDetailLevel() const
	{
		return pft_detail_level_;
	}

	template <typename T>
	void NcGuessVarBase<T>::AddPatchVar(const out_guess::OutputVarPatch& patch_var)
	{

		is_patch_var_ = true;
		patch_detail_level_ = patch_var.GetPatchDetailLevel();

		switch (patch_var.GetPatchDetailLevel())
		{
		case out_guess::PatchDetailLevel::Each_Patch:
			nc_var_->dim_indicies = { 0, 1, 2, 4 };
			//ToDo May fix chunksizes here?! Problem: we need all knowledge about timesteps and gridcells here aswell..
			//var.chunk_sizes = { ... };
			data_2D_ = std::make_shared<ArrayV2D<T>>(nyears_ * time_steps_per_year_, npatch);			
			break;

		case out_guess::PatchDetailLevel::Mean_over_all:
			nc_var_->dim_indicies = { 0, 1, 4 };
			data_2D_ = std::make_shared<ArrayV2D<T>>(nyears_ * time_steps_per_year_, 1);
			break;
		default:;
		}
	}

	template <typename T>
	void NcGuessVarBase<T>::AddPftVar(const out_guess::OutputVarPft& pft_var)
	{

		is_pft_var_ = true;
		pft_detail_level_ = pft_var.GetPftDetailLevel();

		switch (pft_var.GetPftDetailLevel())
		{
		case out_guess::PftDetailLevel::Total:
		{
			nc_var_->dim_indicies = { 0, 1, 4};
			data_2D_ = std::make_shared<ArrayV2D<T>>(nyears_ * time_steps_per_year_, 1);
		}
		break;

		case out_guess::PftDetailLevel::Pfts_Total:
		{
			//ToDo May fix chunksizes here?! Problem: we need all knowledge about timesteps and gridcells here aswell..
			//var.chunk_sizes = { ... };
			nc_var_->dim_indicies = { 0, 1, 3, 4 };
			data_2D_ = std::make_shared<ArrayV2D<T>>(nyears_ * time_steps_per_year_, npft + 1);
		}
		break;

		case out_guess::PftDetailLevel::Invalid:
		{
			string error = "Invalid PftDetailLevel called by NcGuessVarBase";
			throw(error);
		}
		break;

		default:;
		}
	}