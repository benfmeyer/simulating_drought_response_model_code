#include "SmartOutputBuilder.h"
namespace nc_guess
{

	SmartOutputBuilder::SmartOutputBuilder(SmartOutputBuilderOptions& opts) :
		opts(opts), nindivis_alive_stand(0)
	{
	}


	SmartOutputBuilder::~SmartOutputBuilder()
	{

	}

	void SmartOutputBuilder::Define()
	{
		TimeDomain time_domain = TimeDomain::Daily_Monthly_Yearly;

		/// Define your variables below
		cmass_leaf = { "cmass_leaves", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Monthly_Yearly, TimeReductionType::Last };
	    cmass_sap = { "cmass_sap", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		cmass_heart = { "cmass_heart", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		cmass_root = { "cmass_root", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		cmass_total = { "cmass_vegetation", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };

		cmass_agb = { "cmass_agb", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		cmass_bgb = { "cmass_bgb", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		cmass_wagb = { "cmass_wagb", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		wNPP = { "wnpp", "kg C m-2 y-1", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last }; //TP

		cmass_litter = { "cmass_litter", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };

		nmass_leaf = { "nmassleaf", "kg N m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		nmass_total = { "nmasstotal", "kg N m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };

		npp = { "npp", "kg C m-2", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		gpp = { "gpp", "kg C m-2", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };

		fpc = { "fpc", "-", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Last };
		aet_plant = { "aetplant", "mm m-2", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		lai = { "leaf area index", "m2 m-2", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Last };
		dens = { "dens", "-", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Last };
		height = { "height", "m", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Mean };
		vmaxnlim = { "vmaxnlim", "?", PftDetailLevel::Pfts_Total, TimeDomain::Yearly, TimeReductionType::Mean };

		ncohorts = { "ncohorts", "Number of individuals", PatchDetailLevel::Mean_over_all, TimeDomain::Monthly_Yearly, TimeReductionType::Last };
		vpd = { "VPD", "kPa", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };

        wcont = { "wcont", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont1 = { "wcont1", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont2 = { "wcont2", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont3 = { "wcont3", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont4 = { "wcont4", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont5 = { "wcont5", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont6 = { "wcont6", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont7 = { "wcont7", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont8 = { "wcont8", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont9 = { "wcont9", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont10 = { "wcont10", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont11 = { "wcont11", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont12 = { "wcont12", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont13 = { "wcont13", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont14 = { "wcont14", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		wcont15 = { "wcont15", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };

        temp_air = { "temp_air", "K", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		prec = { "prec", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Total };
        sw_rad = { "sw_rad", "J/m2/day", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Total };

		cwd_daily = { "cwd_daily", "mm", PatchDetailLevel::Mean_over_all, TimeDomain::Daily, TimeReductionType::Last };
		cwd_monthly = { "cwd_monthly", "mm", PatchDetailLevel::Mean_over_all,  TimeDomain::Monthly, TimeReductionType::Last };
		mcwd_daily = { "mcwd_daily", "mm", PatchDetailLevel::Mean_over_all, TimeDomain::Yearly, TimeReductionType::Last };
		mcwd_monthly = { "mcwd_monthly", "mm", PatchDetailLevel::Mean_over_all,  TimeDomain::Yearly, TimeReductionType::Last };
		wstress = { "wstress", "range 0-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };


		windspeed = { "windspeed", "m s-1", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Mean };
		et_total = { "total_transpiration", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Total };
		et_soil = { "total_evaporation", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Total };
		et_plant = { "plant_evapotranspiration", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Total};
		runoff = { "total_runoff", "mm", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Total };
		r_het = { "heterotrophic_respiration", "kg C m-2", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Total };
        r_a = {"autotrophic_respiration", "kg C m-2", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		c_fire = { "c_emissions_fire", "kg C m-2", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Last };
		c_fire_no_litter = { "c_emissions_fire_biomass", "kg C m-2", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Last };
		c_fire_only_litter = { "c_emissions_fire_litter", "kg C m-2", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Last };
		c_soil = { "cmass_soil", "kg C m-2", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Last };
		c_litter = { "cmass_litter", "kg C m-2", PatchDetailLevel::Mean_over_all, time_domain, TimeReductionType::Last };

		intercep = { "canopy_interception", "mm", PatchDetailLevel::Mean_over_all , time_domain, TimeReductionType::Total };

		g_C = { "gc", "mm s-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		lambda = { "lambda", "-", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };

		
		phen = { "phen", "-", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		psi_leaf = { "psi_leaf", "MPa", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		psi_soil = { "psi_soil", "MPa", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		psi_xylem = { "psi_xylem", "MPa", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		delta_P = { "delta_psi", "MPa", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
        psi_xyl_min = { "psi_xyl_min", "MPa", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Last };
        psi_leaf_min = { "psi_leaf_min", "MPa", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Last };
        psi_soil_min = { "psi_soil_min", "MPa", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Last };

		J = { "water_flow", "mm d-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		RL = { "rleaf", "m2 MPa s kg-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		Rs = { "rstem", "m2 MPa s kg-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		Rr = { "rroot", "m2 MPa s kg-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		As = { "as", "m2", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };

		g_C_water = { "gc_water", "mm s-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		omega = { "omega", "-", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		et_plant_eql = { "et_plant_eql", "mm d-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		et_plant_imp = { "et_plant_imp", "mm d-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		et_plant_eql_frac = { "et_plant_eql_frac", "mm d-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };
		et_plant_imp_frac = { "et_plant_imp_frac", "mm d-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Total };

		temp_leaf = { "temp_leaf", "�C", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };
		ga = { "ga", "m s-1", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };


		cav_xylem = { "cav_xylem", "-", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Last };
		cav_root = { "cav_root", "-", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Last };
		killed_by_cav = { "killed_by_cav", "-", PftDetailLevel::Pfts_Total, time_domain, TimeReductionType::Mean };


		cmass_lost_bg = { "cmass_loss_bg", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_greff = { "cmass_loss_greff", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_cav = { "cmass_loss_cav", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };


		cmass_lost_heart_bg = { "cmass_loss_heart_bg", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_heart_greff = { "cmass_loss_heart_greff", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_heart_cav = { "cmass_loss_heart_cav", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };

		cmass_lost_sap_bg = { "cmass_loss_sap_bg", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_sap_greff = { "cmass_loss_sap_greff", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_sap_cav = { "cmass_loss_sap_cav", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };

		cmass_lost_leaf_bg = { "cmass_loss_leaf_bg", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_leaf_greff = { "cmass_loss_leaf_greff", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_leaf_cav = { "cmass_loss_leaf_cav", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };

		cmass_lost_root_bg = { "cmass_loss_root_bg", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_root_greff = { "cmass_loss_root_greff", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_lost_root_cav = { "cmass_loss_root_cav", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };

		bminc = { "bminc", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		cmass_repr = { "cmass_repr", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		npp_leaf = { "npp_leaf", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		npp_root = { "npp_root", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		npp_sap = { "npp_sap", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		npp_heart = { "npp_heart", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		npp_debt = { "npp_debt", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		litter_root_inc = { "litter_root_inc", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };
		litter_leaf_inc = { "litter_leaf_inc", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };

		fire_per_pft = { "fire_emissions", "kg C m-2", PftDetailLevel::Pfts_Total, TimeDomain::Yearly , TimeReductionType::Total };


		stand_patch_pft_collectors_.resize(nst + 1);

		for (PatchPftCollector& patch_pft_collector : stand_patch_pft_collectors_)
		{
			//Add variables to the collectors
			patch_pft_collector.AddVar(cmass_leaf);
			patch_pft_collector.AddVar(cmass_heart);
			patch_pft_collector.AddVar(cmass_root);
			patch_pft_collector.AddVar(cmass_sap);
			patch_pft_collector.AddVar(cmass_total);
			patch_pft_collector.AddVar(cmass_agb);
			patch_pft_collector.AddVar(cmass_bgb);
			patch_pft_collector.AddVar(cmass_wagb);
			patch_pft_collector.AddVar(cmass_litter);
			patch_pft_collector.AddVar(wNPP); //TP

			patch_pft_collector.AddVar(nmass_leaf);
			patch_pft_collector.AddVar(nmass_total);
			patch_pft_collector.AddVar(npp);
			patch_pft_collector.AddVar(gpp);
			patch_pft_collector.AddVar(fpc);
			patch_pft_collector.AddVar(aet_plant);
			patch_pft_collector.AddVar(lai);
			patch_pft_collector.AddVar(dens);
			patch_pft_collector.AddVar(height);
			patch_pft_collector.AddVar(vmaxnlim);


			patch_pft_collector.AddVar(phen);
			patch_pft_collector.AddVar(psi_leaf);
			patch_pft_collector.AddVar(psi_soil);
            patch_pft_collector.AddVar(psi_xyl_min);
            patch_pft_collector.AddVar(psi_leaf_min);
            patch_pft_collector.AddVar(psi_soil_min);
			patch_pft_collector.AddVar(psi_xylem);
			patch_pft_collector.AddVar(delta_P);
			//patch_pft_collector_.AddVar(et_plant_total);
			patch_pft_collector.AddVar(g_C);
			patch_pft_collector.AddVar(g_C_water);
			patch_pft_collector.AddVar(lambda);

			patch_pft_collector.AddVar(cmass_lost_bg);
			patch_pft_collector.AddVar(cmass_lost_greff);
			patch_pft_collector.AddVar(cmass_lost_cav);

			patch_pft_collector.AddVar(cmass_lost_leaf_bg);
			patch_pft_collector.AddVar(cmass_lost_leaf_greff);
			patch_pft_collector.AddVar(cmass_lost_leaf_cav);

			patch_pft_collector.AddVar(cmass_lost_sap_bg);
			patch_pft_collector.AddVar(cmass_lost_sap_greff);
			patch_pft_collector.AddVar(cmass_lost_sap_cav);

			patch_pft_collector.AddVar(cmass_lost_heart_bg);
			patch_pft_collector.AddVar(cmass_lost_heart_greff);
			patch_pft_collector.AddVar(cmass_lost_heart_cav);

			patch_pft_collector.AddVar(cmass_lost_root_bg);
			patch_pft_collector.AddVar(cmass_lost_root_greff);
			patch_pft_collector.AddVar(cmass_lost_root_cav);




			patch_pft_collector.AddVar(bminc);
			patch_pft_collector.AddVar(r_a);
			patch_pft_collector.AddVar(cmass_repr);
			patch_pft_collector.AddVar(npp_leaf);
			patch_pft_collector.AddVar(npp_root);
			patch_pft_collector.AddVar(npp_sap);
			patch_pft_collector.AddVar(npp_heart);
			patch_pft_collector.AddVar(npp_debt);
			patch_pft_collector.AddVar(litter_root_inc);
			patch_pft_collector.AddVar(litter_leaf_inc);

			patch_pft_collector.AddVar(fire_per_pft);
			patch_pft_collector.AddVar(wstress);

            patch_pft_collector.AddVar(omega);
            patch_pft_collector.AddVar(et_plant_eql);
            patch_pft_collector.AddVar(et_plant_imp);
            patch_pft_collector.AddVar(et_plant_eql_frac);
            patch_pft_collector.AddVar(et_plant_imp_frac);
            patch_pft_collector.AddVar(temp_leaf);
            patch_pft_collector.AddVar(ga);

            patch_pft_collector.AddVar(cav_xylem);
            patch_pft_collector.AddVar(cav_root);
            patch_pft_collector.AddVar(killed_by_cav);


            patch_pft_collector.AddVar(J);
            patch_pft_collector.AddVar(RL);
            patch_pft_collector.AddVar(Rs);
            patch_pft_collector.AddVar(Rr);
            patch_pft_collector.AddVar(As);


        }


		stand_patch_collectors_.resize(nst + 1);
	

		for (PatchCollector& patch_collector : stand_patch_collectors_)
		{
			patch_collector.AddVar(ncohorts);
			patch_collector.AddVar(vpd);

			patch_collector.AddVar(wcont);
			patch_collector.AddVar(wcont1);
			patch_collector.AddVar(wcont2);
			patch_collector.AddVar(wcont3);
			patch_collector.AddVar(wcont4);
			patch_collector.AddVar(wcont5);
			patch_collector.AddVar(wcont6);
			patch_collector.AddVar(wcont7);
			patch_collector.AddVar(wcont8);
			patch_collector.AddVar(wcont9);
			patch_collector.AddVar(wcont10);
			patch_collector.AddVar(wcont11);
			patch_collector.AddVar(wcont12);
			patch_collector.AddVar(wcont13);
			patch_collector.AddVar(wcont14);
			patch_collector.AddVar(wcont15);

			patch_collector.AddVar(temp_air);
			patch_collector.AddVar(prec);
			patch_collector.AddVar(sw_rad);
			patch_collector.AddVar(windspeed);
			patch_collector.AddVar(et_total);
			patch_collector.AddVar(et_plant);
			patch_collector.AddVar(et_soil);
			patch_collector.AddVar(intercep);
			patch_collector.AddVar(cwd_daily);
			patch_collector.AddVar(cwd_monthly);
			patch_collector.AddVar(mcwd_daily);
			patch_collector.AddVar(mcwd_monthly);
			patch_collector.AddVar(runoff);
			patch_collector.AddVar(r_het);
			patch_collector.AddVar(c_fire);
			patch_collector.AddVar(c_fire_no_litter);
			patch_collector.AddVar(c_fire_only_litter);
			patch_collector.AddVar(c_litter);
			patch_collector.AddVar(c_soil);
		}
	}

	void SmartOutputBuilder::SetPatchValue(const Patch& patch)
	{
		double npatch_d = npatch;
		int stand_id = patch.stand.id;
		PatchCollector& patch_collector = stand_patch_collectors_[stand_id];


        /// total soil water content (mm)
        double sw_tot;
        double swl[NSOILLAYER];

        for (int l = 0; l < NSOILLAYER; ++l) {
            swl[l] = patch.soil.get_layer_soil_water(l) * patch.soil.soiltype.awc[l];
            sw_tot += swl[l];
        }

		patch_collector.SetValuePerPatch(wcont,   sw_tot / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont1,  swl[0] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont2,  swl[1] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont3,  swl[2] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont4,  swl[3] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont5,  swl[4] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont6,  swl[5] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont7,  swl[6] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont8,  swl[7] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont9,  swl[8] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont10, swl[9] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont11, swl[10] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont12, swl[11] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont13, swl[12] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont14, swl[13] / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(wcont15, swl[14] / npatch_d, patch.id, date);


        patch_collector.SetValuePerPatch(ncohorts, patch.vegetation.nobj / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(vpd, patch.get_climate().vpd / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(temp_air, patch.get_climate().temp / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(prec, patch.get_climate().prec / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(sw_rad, patch.get_climate().rad / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(windspeed, patch.get_climate().windspeed / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(et_total, patch.et_total / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(et_plant, patch.et_plant / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(et_soil, patch.et_soil / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(intercep, patch.intercep / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(runoff, patch.arunoff / npatch_d, patch.id, date);

		patch_collector.SetValuePerPatch(r_het, patch.fluxes.get_daily_flux(Fluxes::SOILC, date.day) / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(c_fire, patch.fluxes.get_daily_flux(Fluxes::FIREC, date.day) / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(c_fire_no_litter, patch.fluxes.get_daily_flux(Fluxes::FIREC_NO_LITTER, date.day) / npatch_d, patch.id, date);
		patch_collector.SetValuePerPatch(c_fire_only_litter, patch.fluxes.get_daily_flux(Fluxes::FIREC_ONLY_LITTER, date.day) / npatch_d, patch.id, date);

        patch_collector.SetValuePerPatch(cwd_daily, patch.get_climate().cwd_daily_3, patch.id, date);
        patch_collector.SetValuePerPatch(cwd_monthly, patch.get_climate().cwd_monthly_100, patch.id, date);
        patch_collector.SetValuePerPatch(mcwd_monthly, patch.get_climate().mcwd_monthly_100, patch.id, date);
        patch_collector.SetValuePerPatch(mcwd_daily, patch.get_climate().mcwd_daily_3 , patch.id, date);


		if (!ifcentury)
		{
			double c_fast = patch.soil.cpool_fast / npatch_d;
			double c_slow = patch.soil.cpool_slow / npatch_d;
			
			patch_collector.SetValuePerPatch(c_soil, c_fast + c_slow, patch.id, date);
		}

		else
		{
			double surfsoillitterc = 0.0;
			double cwdc = 0.0;
			double centuryc = 0.0;
			
			for (int r = 0; r < NSOMPOOL; r++) {

				if (r == SURFMETA || r == SURFSTRUCT || r == SOILMETA || r == SOILSTRUCT) {
					surfsoillitterc += patch.soil.sompool[r].cmass / npatch_d;
				}
				else if (r == SURFFWD || r == SURFCWD) {
					cwdc += patch.soil.sompool[r].cmass / npatch_d;
				}
				else {
					centuryc += patch.soil.sompool[r].cmass / npatch_d;
				}
			}

			patch_collector.SetValuePerPatch(c_soil, centuryc, patch.id, date);
			patch_collector.SetValuePerPatch(c_litter, (surfsoillitterc + cwdc ), patch.id, date);
			
		}
		



		

	}

	void SmartOutputBuilder::SetPftValue(const Individual& indiv, const Patch& patch, const Pft& pft)
	{
		const double npatch_d = npatch;
		const int stand_id = patch.stand.id;
		PatchPftCollector& patch_pft_collector = stand_patch_pft_collectors_[stand_id];

		patch_pft_collector.AccValPerPatch(cmass_leaf, indiv.cmass_leaf_today()/ npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(cmass_sap, indiv.cmass_sap / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(cmass_heart, indiv.cmass_heart / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(cmass_root, indiv.cmass_root / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(cmass_total, indiv.ccont() / npatch_d, pft.id, date);
		//patch_pft_collector.AccValPerPatch(wNPP, indiv.wNPP / npatch_d, pft.id, date); //TP
		
		//patch_pft_collector.AccValPerPatch(cmass_agb, indiv.cmass_agb() / npatch_d, pft.id, date);
		//patch_pft_collector.AccValPerPatch(cmass_bgb, indiv.cmass_bgb() / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(cmass_wagb, (indiv.cmass_sap + indiv.cmass_heart - indiv.cmass_debt) / npatch_d, pft.id, date);


		patch_pft_collector.AccValPerPatch(nmass_leaf, indiv.nmass_leaf / indiv.cton_leaf_aavr * M2TOHA / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(nmass_total, indiv.ncont() * M2TOHA / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(npp, indiv.dnpp / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(gpp, indiv.dgpp / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(fpc, indiv.fpc / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(aet_plant, indiv.aet / npatch_d, pft.id, date);
		patch_pft_collector.AccValPerPatch(lai, indiv.lai / npatch_d, pft.id, date);//AHES
		patch_pft_collector.AccValPerPatch(vmaxnlim, indiv.avmaxnlim * indiv.cmass_leaf / npatch_d, pft.id, date);


		if (pft.lifeform == TREE)
		{
			const double nindivs_stand = static_cast<double>(nindivis_alive_stand);
			const double nindivs_stand_pft = static_cast<double>(nindivis_alive_stand_pft[pft.id]);


			patch_pft_collector.AccValPerPatch(dens, indiv.densindiv, pft.id, date);
			patch_pft_collector.AccValPerPatch(height, indiv.height * indiv.densindiv, pft.id, date);

			// Todo this section needs some improvements
			// Currently we have three possibilities
			// 1. Calculate the mean per individual over all patches --> devide value by n_indivs_stand
			// 2. Calculate the accumulated total per patch over all indivis per patch --> devide by npatch
			// 3. Calculate the accumulated value over all patches --> do nothing



            patch_pft_collector.AccValPerPatch(r_a, indiv.resp/nindivs_stand_pft, pft.id, date );
            patch_pft_collector.AccValPerPatch(phen, indiv.phen / nindivs_stand_pft, pft.id, date);

            patch_pft_collector.AccValPerPatch(wstress, indiv.wstress / nindivs_stand_pft, pft.id, date);
			patch_pft_collector.AccValPerPatch(lambda, indiv.lambda / nindivs_stand_pft, pft.id, date);

            //Update some values only for the appropriate hydraulic system
            switch (hydraulic_system)
            {
                case STANDARD: {
                    patch_pft_collector.AccValPerPatch(g_C, indiv.patchpft().gcbase / nindivs_stand_pft, pft.id, date);
                }
                    break;
                case MONTEITH_SUP_DEM:
                    break;
                case VPD_BASED_GC:{
                    patch_pft_collector.AccValPerPatch(omega,indiv.hydraulic_data.omega/ nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(et_plant_eql, indiv.hydraulic_data.et_plant_eql / npatch_d, pft.id, date);
                    patch_pft_collector.AccValPerPatch(et_plant_imp, indiv.hydraulic_data.et_plant_imp / npatch_d, pft.id, date);
                    //patch_pft_collector.AccValPerPatch(et_plant_eql_frac, indiv.hydro_vpd_ptr->get_et_plant_eql_frac() / npatch_d, pft.id, date);
                    //patch_pft_collector.AccValPerPatch(et_plant_imp_frac, indiv.hydro_vpd_ptr->get_et_plant_imp_frac() / npatch_d, pft.id, date);
                    patch_pft_collector.AccValPerPatch(temp_leaf, indiv.hydraulic_data.temp_leaf/ nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(ga, indiv.hydraulic_data.ga_run / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(g_C, indiv.hydraulic_module->get_gC() / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(J, indiv.hydraulic_data.J / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(RL, indiv.hydraulic_data.R_L * indiv.fpc, pft.id, date);
                    patch_pft_collector.AccValPerPatch(Rs, indiv.hydraulic_data.R_s * indiv.fpc, pft.id, date);
                    patch_pft_collector.AccValPerPatch(Rr, indiv.hydraulic_data.R_r * indiv.fpc, pft.id, date);
                    patch_pft_collector.AccValPerPatch(As, indiv.hydraulic_data.A_s * indiv.fpc, pft.id, date);
                    patch_pft_collector.AccValPerPatch(psi_leaf, indiv.hydraulic_data.psi_L / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(psi_soil,  indiv.hydraulic_data.psi_sw/ nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(psi_xylem,  indiv.hydraulic_data.psi_xyl/ nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(psi_xyl_min, indiv.hydraulic_data.psi_xyl_min / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(psi_leaf_min, indiv.hydraulic_data.psi_L_min / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(psi_soil_min,  indiv.hydraulic_data.psi_sw_min/ nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(delta_P, indiv.hydraulic_data.delta_Psi / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(cav_xylem, indiv.hydraulic_data.kappa_s_min / nindivs_stand_pft, pft.id, date);
                    patch_pft_collector.AccValPerPatch(cav_root, indiv.hydraulic_data.kappa_r / nindivs_stand_pft, pft.id, date);
                }
                    break;
                default:
                    break;
            }
	
		}
	}
}

