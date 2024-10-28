///////////////////////////////////////////////////////////////////////////////////////
/// \file SmartOutputBuilder.h
/// \brief Control panel for setting up/modifing output
///  The main SmartOutput class derives from this class 
/// \author Phillip Papastefanou
/// $Date: 2017-02-11 23:17:02 +0100 (Sun, 11 Feb 2017) $
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "guess.h"
#include "PatchPftCollector.h"
#include "PatchCollector.h"
#include "internal_out/SingleWriterAnnualy.h"
#include "internal_out/SingleWriterMonthly.h"
#include "internal_out/SingleWriterDaily.h"
#ifdef HAVE_PNETCDF
#include "internal_out/ParallelWriterAnnualy.h"
#include "internal_out/ParallelWriterMonthly.h"
#include "internal_out/ParallelWriterDaily.h"
#endif
#include "SmartOutputBuilderOptions.h"

class Patch;
class Stand;
class Pft;

namespace nc_guess
{
	using namespace out_guess;

	class SmartOutputBuilder
	{
	public:
		SmartOutputBuilder(SmartOutputBuilderOptions& opts);
		virtual ~SmartOutputBuilder();

	protected:
		/// The main intance of the options
		SmartOutputBuilderOptions& opts;

		/// Square meter to hectar conversion constant
		const double M2TOHA = 10000.0;

		/// Active, alive Individuals for each stand
		size_t nindivis_alive_stand;

		/// Active, alive Individuals for each stand of each Pft
		std::vector<int> nindivis_alive_stand_pft;

		/**
		 * \brief Define all output variable and add them to the collectors
		 */
		void Define();

		/**
		 * \brief Function where to set the value of each OutputVarPatch
		 * \param patch actual patch reference
		 */
		void SetPatchValue(const Patch& patch);
		/**
		 * \brief Function where to set the value of each OutputVarPft
		 * \param indiv Selected cohort/indiv
		 * \param patch Selected patch
		 * \param pft Selected pft
		 */
		void SetPftValue(const Individual& indiv, const Patch& patch, const Pft& pft);
				
		/// annual writer
		std::unique_ptr<BaseWriter> awriter;
		/// monthly writer
		std::unique_ptr<BaseWriter> mwriter;
		/// daily writer
		std::unique_ptr<BaseWriter> dwriter;
		
		/// vector of references to above writers
		vector<std::reference_wrapper<BaseWriter>> writers;

		/// Variable collector of OutputVarPft
		std::vector<PatchPftCollector> stand_patch_pft_collectors_;

		/// Variable collector of OutputVarPatch
		std::vector<PatchCollector> stand_patch_collectors_;

		

		/// Output variables
		/// Define your variables below!
		OutputVarPft cmass_root;
		OutputVarPft cmass_leaf;
		OutputVarPft cmass_heart;
		OutputVarPft cmass_sap;
		OutputVarPft cmass_agb;
		OutputVarPft cmass_total;
		OutputVarPft cmass_bgb;
		OutputVarPft cmass_wagb;
		OutputVarPft wNPP; //TP

		OutputVarPft cmass_litter;


		OutputVarPft nmass_leaf;
		OutputVarPft nmass_total;

		OutputVarPft npp;
		OutputVarPft gpp;

		OutputVarPft fpc;
		OutputVarPft aet_plant;
		OutputVarPft lai;
		OutputVarPft dens;
		OutputVarPft height;
		OutputVarPft vmaxnlim;

		OutputVarPft g_C;
		OutputVarPft lambda;

		OutputVarPft phen;
		OutputVarPft psi_leaf;
		OutputVarPft psi_soil;
		OutputVarPft psi_xylem;
        OutputVarPft psi_xyl_min;
        OutputVarPft psi_soil_min;
        OutputVarPft psi_leaf_min;
		OutputVarPft delta_P;
        OutputVarPft wstress;



		OutputVarPft J;
		OutputVarPft Rs;
		OutputVarPft Rr;
		OutputVarPft RL;
		OutputVarPft As;

		
		OutputVarPft cmass_lost_bg;
		OutputVarPft cmass_lost_greff;
		OutputVarPft cmass_lost_cav;

		OutputVarPft cmass_lost_heart_bg;
		OutputVarPft cmass_lost_heart_greff;
		OutputVarPft cmass_lost_heart_cav;

		OutputVarPft cmass_lost_sap_bg;
		OutputVarPft cmass_lost_sap_greff;
		OutputVarPft cmass_lost_sap_cav;

		OutputVarPft cmass_lost_leaf_bg;
		OutputVarPft cmass_lost_leaf_greff;
		OutputVarPft cmass_lost_leaf_cav;

		OutputVarPft cmass_lost_root_bg;
		OutputVarPft cmass_lost_root_greff;
		OutputVarPft cmass_lost_root_cav;

		OutputVarPft bminc;
		OutputVarPft cmass_repr;
		OutputVarPft npp_leaf;
		OutputVarPft npp_root;
		OutputVarPft npp_sap;
		OutputVarPft npp_heart;
		OutputVarPft npp_debt;
		OutputVarPft litter_root_inc;
		OutputVarPft litter_leaf_inc;
		OutputVarPft fire_per_pft;
        OutputVarPft r_a;



		//Variables only dealing with HydroDynamicsVPD
		OutputVarPft omega;
		OutputVarPft et_plant_eql;
		OutputVarPft et_plant_imp;
		OutputVarPft et_plant_eql_frac;
		OutputVarPft et_plant_imp_frac;
		OutputVarPft temp_leaf;		
		OutputVarPft ga;
		OutputVarPft g_C_water;

		OutputVarPft cav_xylem;
		OutputVarPft cav_root;
		OutputVarPft killed_by_cav;


        OutputVarPatch vpd;
        OutputVarPatch prec;
        OutputVarPatch windspeed;
        OutputVarPatch temp_air;
        OutputVarPatch sw_rad;

		OutputVarPatch ncohorts;

		OutputVarPatch wcont;
        OutputVarPatch wcont1;
        OutputVarPatch wcont2;
        OutputVarPatch wcont3;
        OutputVarPatch wcont4;
        OutputVarPatch wcont5;
        OutputVarPatch wcont6;
        OutputVarPatch wcont7;
        OutputVarPatch wcont8;
        OutputVarPatch wcont9;
        OutputVarPatch wcont10;
        OutputVarPatch wcont11;
        OutputVarPatch wcont12;
        OutputVarPatch wcont13;
        OutputVarPatch wcont14;
        OutputVarPatch wcont15;

        OutputVarPatch runoff;
		OutputVarPatch r_het;

		OutputVarPatch c_fire;
		OutputVarPatch c_fire_no_litter;
		OutputVarPatch c_fire_only_litter;
		OutputVarPatch c_soil;
		OutputVarPatch c_litter;
		

		OutputVarPatch et_total;
		OutputVarPatch et_plant;
		OutputVarPatch et_soil;
		OutputVarPatch intercep;

		OutputVarPatch cwd_daily;
		OutputVarPatch cwd_monthly;
		OutputVarPatch mcwd_daily;
		OutputVarPatch mcwd_monthly;


	};
}

