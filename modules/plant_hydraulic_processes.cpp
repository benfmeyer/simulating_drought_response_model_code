#include "guess.h"
#include <iostream>
#include <cmath>
#include "plant_hydraulic_processes.h"

SubModule::SubModule()
{
}

SubModule::~SubModule()
{
}


PsiSoilLeafSystem::PsiSoilLeafSystem(Individual& indiv, IHydroDynamics& hydro_dynamics) :
	indiv(indiv),
	ihyd(indiv.hydraulic_data),
	patch(indiv.vegetation.patch),
	pft(indiv.pft),
	// Two steps to find the root seem to be sufficient for many cases, even at stiff equations!
	// Todo: Make the niterations and error tolerance parameters of the insfiles
	MidpointImplicit(5, 0.01),
	EulerExplicit()
{
	//Initialise with the minimum (optimum) value for the forcing pressure
	ihyd.psi_L = -pft.delta_psi_max;
}

PsiSoilLeafSystem::~PsiSoilLeafSystem()
{
}

void PsiSoilLeafSystem::ConnectTo(SubModule* subModule)
{
	///This might be removed in future. We leave it here, so that nobody accidently uses this function as long as there is no submodule connection
	fail("Invalid hydraulic submodule connection.");
}

void PsiSoilLeafSystem::UpdatePsiSoil()
{
	// Copy prev value to new value we need this for the psi_leaf water potentials
	ihyd.psi_sw_prev = ihyd.psi_sw;

	//Weighted psi_soil by rooting depth (according to Hickler et al. 2006)
	ihyd.psi_sw = 0.0;
	for (size_t s = 0; s < NSOILLAYER; ++s)
	{
		ihyd.psi_sw += pft.rootdist[s] * patch.soil.psi_s[s];
	}

    if (ihyd.gC > indiv.pft.gmin)
        ihyd.psi_sw_min = std::min(ihyd.psi_sw, ihyd.psi_sw_min);
    if (date.day == 0)
        ihyd.psi_sw_min = 0.0;
}

void PsiSoilLeafSystem::update_water_potentials(const double psi_leaf)
{
	// Update psi leaf using a combination of two ODE-Solver (to increase performance)
	// Ensure that psi_leaf in summergreens is calculated properly when leaves are off i.E. "leaf/twig" water potential
	// is assumed to follow soil water potential leading to no pressure gradient within tree
	ihyd.psi_L = std::min(psi_leaf * indiv.phen, ihyd.psi_sw);

	// Update forcing pressure
	// Todo introduce the gravitational contant
	ihyd.delta_Psi = - (ihyd.psi_L -  ihyd.psi_sw - RHO_WATER * 9.81 * indiv.height / 1.0E6);

	// Delta P should not be negative as this would cause the water to flow down the plants...
	if (ihyd.delta_Psi < 0.0)
		ihyd.delta_Psi = 0.0;

	/// Derive xylem water potential based on Fisher et al. 2006, PCE
	// be factors variet between 0.1-0.2 (wet season) and about 0.6-0.9 (dry season)
	// We use the conservative of 0.5 which is equivalent to what I came up with...
	const double b = 0.5;
	ihyd.psi_xyl = b * (ihyd.psi_L -  ihyd.psi_sw) +  ihyd.psi_sw - RHO_WATER * 9.81 * indiv.height / 1.0E6;

    if (ihyd.gC > indiv.pft.gmin) {
        ihyd.psi_xyl_min = std::min(ihyd.psi_xyl, ihyd.psi_xyl_min);
        ihyd.psi_L_min = std::min(ihyd.psi_L, ihyd.psi_L_min);
    }

    if (date.day == 0) {
        ihyd.psi_xyl_min = 0.0;
        ihyd.psi_L_min = 0.0;
    }
    // Also justifiable according to Ishihara and Hirasawa (1978)
    // psi_xyl = psi_L;
}


double PsiSoilLeafSystem::InternalPsiLeaf()
{
	// We are always starting by the point (0, psi_L)
	double x0 = 0.0;
	double y0 = ihyd.psi_L;

	// We want to get the point (1, psi_l_new)
	// It seems that a stepside of 1.0 (n = 1) is enough to get sufficent precision
	// Or: If we run into a stiff system, even a lower steplength will increase precision
	double x = 1.0;
	double stepsize = 1.0; //Todo make this flexible with regard to timesteps

	return  MidpointImplicit::ODESolve(x0, y0, x, stepsize);
}

double PsiSoilLeafSystem::dpsi_leaf_dt(const double t, const double psi_leaf)
{
	return dydx(t, psi_leaf);
}

double PsiSoilLeafSystem::ip_psi_soil(const double time) const
{
	return (ihyd.psi_sw - ihyd.psi_sw_prev)*time + ihyd.psi_sw_prev;
}


double PsiSoilLeafSystem::dydx(const double x, const double y)
{
	return  ip_psi_soil(x) * (1 - pft.isohydricity) - y - pft.delta_psi_max;
}

double PsiSoilLeafSystem::d2ydx2(const double x, const double y)
{
	return -1.0 + ip_psi_soil(x)* (1 - pft.isohydricity);
}

ReversePsiLeafSystem::ReversePsiLeafSystem(): BisectionSolver(10, 0.1),
											  psi_soil_leaf_system_(nullptr),
											  cavitation_resistance_system(nullptr),
											  evapotranspiration_system(nullptr)
{
}

void ReversePsiLeafSystem::ConnectTo(SubModule* subModule)
{
	if (dynamic_cast<CavitationResistanceSystem*>(subModule) != nullptr)
	{
		cavitation_resistance_system = dynamic_cast<CavitationResistanceSystem*>(subModule);
	}
	else if (dynamic_cast<PsiSoilLeafSystem*>(subModule) != nullptr)
	{
		psi_soil_leaf_system_ = dynamic_cast<PsiSoilLeafSystem*>(subModule);
	}
	else if (dynamic_cast<EvapotranspirationSystem*>(subModule) != nullptr)
	{
		evapotranspiration_system = dynamic_cast<EvapotranspirationSystem*>(subModule);
	}
	else
	{
		fail("Invalid hydraulic submodule.");
	}

}

void ReversePsiLeafSystem::UpdatePsiLeaf(double gC, double psiL_min)
{
	//gC_match = gC;
	//double psi_LNew = FindRoot(psiL_min, psi_soil_leaf_system_->patch.soil.psi_sw);
}

double ReversePsiLeafSystem::f(const double x)
{
	//psi_soil_leaf_system_->UpdatePsiLeafManually(x);
	cavitation_resistance_system->UpdateCavitation();
	cavitation_resistance_system->UpdateResistance();
	evapotranspiration_system->UpdateConductivity();
	return 0.0;
	//return std::abs(ihyd.gC - gC_match);
}


CavitationResistanceSystem::CavitationResistanceSystem(Individual& indiv, IHydroDynamics& hydro_dynamics) :
	indiv(indiv),
	patch(indiv.vegetation.patch),
	pft(indiv.pft),
	ihyd(indiv.hydraulic_data),
	cav_xylem_calc(pft.cav_slope, pft.psi50_xylem),
	cav_leaf_calc(pft.cav_slope, pft.psi50_xylem),
	cav_root_calc(pft.cav_slope, pft.psi50_xylem / PSI_50_RESCALE),
	A_s(hydro_dynamics.A_s),
	eta_s(hydro_dynamics.eta_soil),
	eta_a(hydro_dynamics.eta_air)
{

}

CavitationResistanceSystem::~CavitationResistanceSystem()
{
}

void CavitationResistanceSystem::UpdateCavitation()
{
	// On the first day of existence, use maximum conductivity
    if (ihyd.gC > indiv.pft.gmin)
	    ihyd.kappa_s_today = cav_xylem_calc.Get(ihyd.psi_xyl);
    else
        ihyd.kappa_s_today = 0.0;

	// if we have stronger cavitation then yesterday use the new value for cavitation
	// cavitation must be greater than the no damage value to account for damage
	if (ihyd.kappa_s_today > ihyd.kappa_s_min && ihyd.kappa_s_today > pft.kappa_no_damage)
	{
		//kappa_s = kappa_s_today;
		ihyd.kappa_s_min = ihyd.kappa_s_today;
	}


	// Leaf and root cavitation are extremly simplyfied and just assumend to be higher than xylem cavitation
	// this one AGU-dude said it is about 2 times higher than xylem cavitation
	// Leaf and root cavitation can be repaired on a daily level,
	ihyd.kappa_r = cav_root_calc.Get(ihyd.psi_sw);
	// Todo Think about linking it to leaf longevitiy (same with the roots)
	ihyd.kappa_L = cav_leaf_calc.Get(ihyd.psi_L);

	// If cavitation is smaller than no damage value
	// -> No cavitation of roots
	// Todo add reference
	if (ihyd.kappa_r < pft.kappa_no_damage)
		ihyd.kappa_r = 0.0;

	// If cavitation is smaller than no damage value
	// -> No cavitation of leafs
	if (ihyd.kappa_L < pft.kappa_no_damage)
		ihyd.kappa_L = 0.0;

}

void CavitationResistanceSystem::UpdateResistance()
{
	// Shoot / sapwood / xylem resistance
	// Refactored from Hickler 2006
	// Using specific sapwood resistance per stem crossectional area instead
	//double A_s = indiv.cmass_sap
	// Sapwood area
	 ihyd.A_s = (indiv.cmass_sap * 2.0) / pft.wooddens / indiv.height;

	const double shoot_denom = pft.ks_max * (1.0 - ihyd.kappa_s_min) * ihyd.A_s / indiv.height * eta_a;
	if (!negligible(shoot_denom) && !negligible(indiv.height))
		ihyd.R_s = 1.0 / shoot_denom;
	else
		ihyd.R_s = 0.0;

	// Root resistance
	// Refactored from Hickler 2006
    // BEN M. (2024) - Refactored to allow kr_max to be supplied in common units (kg m-1 s-1 MPa-1).
    // 1. Conversion from cmass root in kg C m-2 to root biomass volume in m3:
    //    roughly 50% of biomass is carbon --> multiply cmass_root by 2.0
    //    root tissue density is analogous to wood density
    // 2. Conversion from root biomass volume to root area:
    //    we model fine roots to a maximum depth of 1.5m --> divide root volume by 1.5
	if (!negligible(indiv.cmass_root)) {
        double root_volume = (indiv.cmass_root * 2.0) / pft.rtd;
        double root_area = root_volume / 1.5;
        ihyd.R_r = 1.0/(pft.kr_max * (1.0 - ihyd.kappa_r) *  root_area/1.5  * eta_s);
    }
	// Leaf resistance
	// Refactored from Hickler 2006 using mmol
	// The first factor 1000.0 is to convert from mmol -> mol
	// The second factor 1000.0  from g --> kg
	if (!negligible(indiv.fpc))
		ihyd.R_L = 1.0/(pft.kl_max / 1000.0 * MOLAR_MASS_WATER / 1000.0 * (1.0 - ihyd.kappa_L) * indiv.cmass_leaf * pft.sla);
    else
        ihyd.R_L = 0.0;


	// Sum up total resistances
	ihyd.R_total = ihyd.R_r + ihyd.R_s + ihyd.R_L;
    double frac_s = ihyd.R_s * indiv.fpc;
    double frac_r = ihyd.R_r * indiv.fpc;
    double frac_L = ihyd.R_L * indiv.fpc;
    double tt = frac_s + frac_r + frac_L;

}


void CavitationResistanceSystem::ConnectTo(SubModule* subModule)
{
	if (dynamic_cast<const PsiSoilLeafSystem*>(subModule) != nullptr)
	{
		psi_soil_leaf_system = dynamic_cast<const PsiSoilLeafSystem*>(subModule);
	}
	else
	{
		fail("Invalid hydraulic submodule.");
	}
}

EvapotranspirationSystem::EvapotranspirationSystem(Individual& indiv, HydroDynamicsVPD& hydro_dynamics):
		indiv(indiv),
		patch(indiv.vegetation.patch),
		pft(indiv.pft),
		ihyd(indiv.hydraulic_data),
		eet_net_veg(patch.eet_net_veg),
		rad(patch.get_climate().rad),
		windspeed(patch.get_climate().windspeed),
		temp_air(patch.get_climate().temp),
		vpd(patch.get_climate().vpd),
		daylength(patch.get_climate().daylength),
		BisectionSolver(6, 0.1)
{
}

EvapotranspirationSystem::~EvapotranspirationSystem()
{
}

void EvapotranspirationSystem::ConnectTo(SubModule* subModule)
{
	if (dynamic_cast<PsiSoilLeafSystem*>(subModule) != nullptr)
	{
		psi_soil_leaf_system = dynamic_cast<PsiSoilLeafSystem*>(subModule);
	}

	else if (dynamic_cast<CavitationResistanceSystem*>(subModule) != nullptr)
	{
		cav_resistance_system = dynamic_cast<CavitationResistanceSystem*>(subModule);
	}

//	else if (dynamic_cast<ReversePsiLeafSystem*>(subModule) != nullptr)
//	{
//		reverse_psi_leaf_system = dynamic_cast<ReversePsiLeafSystem*>(subModule);
//	}
	else
	{
		fail("Invalid hydraulic submodule.");
	}
}

void EvapotranspirationSystem::UpdateConductivity()
{
	// Forcing pressure [MPa]
	double deltaPsi = ihyd.delta_Psi;

	// Total resistance [m2 MPa s kg-1]
	double r_total = ihyd.R_total;

	// Hickler et al. 2006 similar calculation of water flow in mm/day without fpc
	// only used for evaluation yet
	// Todo Check if this is really the potential waterflow
	ihyd.J  = (deltaPsi / r_total) * 86400.0;

	// Sanity check to avoid negative water flows
	if (ihyd.J < 0.0){
		fail("EvapotranspirationSystem::UpdateConductivity Error: Negative waterflow deteced.");
	}
	// If the air is fully saturated ...the CO2 gradient is still in place.
	// Even that the stomatal conductance is low for water because of the
	// low diffusion rate of water vapor is the atmosphere. In this case
	// we should consider that the stomata are open and the loss of water
	// vapor is low
	double vpd_nonzero{vpd};
	//gC_water must be controled here
	ihyd.low_diffusion_rate_w = false;
	if (vpd > -0.001)
	{
		vpd_nonzero = -0.001;
		ihyd.low_diffusion_rate_w = true;
	}
		// Need to avoid a zero division here.
		// If the air is fully saturated, then difusion of water is low. Nonetheless diffusion of CO2 is still in place
		// ihyd.gC = indiv.gpterm; // Set gc to gmax
		// ihyd.gC = 0.0

	// Canopy conductance calculation assuming supply-demand principle
	// e.g J == E_imp and solved for gC
	ihyd.gC = LAMBDA * GAMMA / CP_AIR * RHO_WATER / RHO_AIR * deltaPsi / r_total / (-vpd_nonzero);

}

void EvapotranspirationSystem::CheckForAtmosphericReduction()
{
	// Limit gC with gmax -- GPTERM
	// gC is used to calculate water stressed npp water stressed
	// gC_water is used to calculate transpiration

	// Note that both variables are conductances to water. In canexp.cpp (npp function) gC is converted to gcphot
	// i.e., stomatal conductance to CO2
	// This is why we do not add gmin to gC. gmin is influential only for transpiration.
	double g{ihyd.gC}; //store

	if (ihyd.gC >= indiv.gpterm)
	{
		ihyd.gC = indiv.gpterm;
		ihyd.waterStressedGC = false;
	}

	// Water stressed photosynthesis
	// gc is smaller then photosynthesis optimum
	else
	{
		ihyd.waterStressedGC = true;
	}

	// can gc be lower than gmin?
	// if low stomatal conductance, we consider that we also have low C assimilation.
	// if gC is lower that gmin, then gC is lower than gmin.
	// Note that this will only affect gcphot when waterStressedGC is true
	// Otherwise gpterm is low and the limitation for phot is light or other stuf than water
	// if (ihyd.gC < indiv.pft.gmin) ihyd.gC = indiv.pft.gmin;

	// Update individual
	indiv.wstress = ihyd.waterStressedGC;

	// The transpiration function uses gc water to do its calculations
	// gC_water must be scaled to canopy and have the unit transformed
	// to m/s to satisfy transpiration() needs. From the reference:
	// "gc is the product of a "tree canopy average stomatal conductance and leaf area index)"
	// gmin is added here. Also counts for transpiration
	// lai_indiv_today() accounts phen
	// If vpd is low (i.e water vapor saturation) then we should expect low diffusion rates
	// and less loss of water vapor from leaves. Even that the stomata are fully open.
	// Set it to pft.gmin (gLOW)
	/*Körner, C.: Leaf Diffusive Conductances in the Major Vegetation Types of the
	 Globe, in: Ecophysiology of Photosynthesis, edited by: Schulze, E.-D.,
	 and Caldwell, M. M., Springer Berlin Heidelberg, Berlin, Heidelberg,
	 463-490, https://doi.org/10.1007/978-3-642-79354-7_22, 1995.*/
	if(ihyd.low_diffusion_rate_w)
	{
		ihyd.gC_water = indiv.pft.gmin * indiv.lai_indiv_today() * 1e-3;
	}
	else
	{
		ihyd.gC_water = (indiv.pft.gmin + g) * indiv.lai_indiv_today() * 1e-3; // convert to m/s for use in transpiration calculation
	}
}

void EvapotranspirationSystem::UpdateTranspiration()
{
	// Initialise ga with the standard pft value
	ihyd.ga_run = pft.ga;

	// Initialise previous leaf temperature with something arbitiary
	temp_leaf_prev = -9999.0;

	transpiration();
	leaf_temperature();

	const int maxtries = 6;
	int counter = 0;
	const double leaf_temp_eps = 0.1;

	while (counter < maxtries && temp_leaf_delta > leaf_temp_eps)
	{
		aerodynamic_conductance_leuning();
		transpiration();
		leaf_temperature();

		counter++;
	}

	// Update 02.12.2021
	// Get evapotranspiration on FPC level - No more
	double et_sum = ihyd.et_plant_total;

	// we have higher transpiration than can acutally be transported through the plants
	if (et_sum > ihyd.J){
		// Todo are you waterstressed in this situation?!
		ihyd.et_plant_total = ihyd.J;
	}

	// We have lower evapotranspiration compared to J (et_sum < ihyd.J)
	// In this case we downregulate leaf water potential and J to match with
	else{
		// We avoid weighting variables by FPC in the preceding lines to not mess with finding roots
		// We put et_plant_total on FPC level in the very last line of this function
		J_target = et_sum;
		FindRoot(ihyd.psi_sw, ihyd.psi_L);
	}

	if(ihyd.et_plant_total < 0.0){
		fail("EvapotranspirationSystem::UpdateTranspiration Error: Negative transpiration.");
	}
	// weight by FPC at the end of caclulations... fpc_today accounts phenology
	ihyd.et_plant_total *= indiv.fpc_today();
    double test = ihyd.et_plant_total;
	}


void EvapotranspirationSystem::transpiration()
{
	/*
	Köstner, B. M. M., Schulze, E. D., Kelliher, F. M., Hollinger, D. Y., Byers, J. N.,
	   Hunt, J. E., McSeveny, T. M., Meserth, R., and Weir, P. L.: Transpiration and Canopy
	   Conductance in a Pristine Broad-Leaved Forest of Nothofagus: An Analysis of Xylem Sap
	   Flow and Eddy Correlation Measurements, Oecologia, 91, 350-359,
	   https://doi.org/10.1007/BF00317623, 1992.
	*/

	//// Degree of coupling Köstner et al. 1992, eqn 4. Conductance units m/s
	ihyd.omega = (1.0 + EPS) / (1.0 + EPS + ihyd.ga_run / ihyd.gC_water);

	//// Equilibrium transpiration, here simply adaped from std LPG-GUESS
	ihyd.et_plant_eql = eet_net_veg;

	//// Imposed transpiration, Koestner 1992, eqn 3.
	//// (water that comes from the soil induced by VPD)
	const double cst = RHO_WATER * G_V * (C0 + temp_air);
	ihyd.et_plant_imp = (ihyd.gC_water * -vpd) / cst * 86400 * 1000; // convert m to mm

	//// Instructive form of Penman-Monteith eqn for transpiration, Koestner 1992, eqn 1.
	ihyd.et_plant_total = ihyd.omega * ihyd.et_plant_eql + (1.0 - ihyd.omega) * ihyd.et_plant_imp;
}

void EvapotranspirationSystem::leaf_temperature()
{
	// Canopy temperature is calculated from the air temperature and the energy balance (longwave
	// radiation, shortwave radiation and sensible and latent heat loss).
	// Revised version compared to Arneth et al. (2007) and Schurgers et al. (2011).

	if (indiv.lai_today() <= 1.e-2)
	{
		ihyd.temp_leaf = temp_air;
		temp_leaf_prev = ihyd.temp_leaf;
		temp_leaf_delta = 0.0;
	}

	else
	{
		// Comment by PP:
		// The below code is simply copied and refactored, however the definitions of S_net, L_net, etc... might not
		// be the same as in the comment text. I should ask Almut or Guy, why.


		// leaf temperature is calculated by balancing four fluxes:
		// 1. net SW radiation, computed from the incoming radiation
		//    S_net = -rs_day*fpar/(daylength*3600.)
		// 2. net LW radiation, computed as a first-order Taylor expansion of Stefan-Boltzman law,
		//    which makes it a linear function of the temperature difference deltaT
		//    L_net = 4*emiss_leaf*sigma*(T**3.)*deltaT*phen*lai
		// 3. latent heat, computed from actual evapotranspiration AET
		//    LH = aet*lam/(daylength*3600.)
		// 4. sensible heat, computed as a linear function of the temperat
		//    H = deltaT*rhoair*cp*ga*phen*lai
		//
		const double S_net = rad* indiv.fpar / (daylength*3600.0);
		const double L_net = 4 * EMISS_LEAF*SIGMA*std::pow(temp_air + K2degC, 3.0)*indiv.fpc_today();

		const double LH = ihyd.et_plant_total * LAMBDA * 1000.0 / (daylength*3600.0);
		const double H = RHO_AIR* CP_AIR * 1000.0 * ihyd.ga_run * indiv.lai_today();

		// Update leaf temperature according to Arneth 2007, equation 2.
		ihyd.temp_leaf = temp_air + (S_net - LH) / (L_net + H);

		// Calculate the absolute change in precision compared to the previous step
		temp_leaf_delta = fabs(ihyd.temp_leaf - temp_leaf_prev);

		// Save current leaf temperature as previous leaf temperature
		temp_leaf_prev = ihyd.temp_leaf;
	}
}



double EvapotranspirationSystem::f(const double x) {

	psi_soil_leaf_system->update_water_potentials(x);
	cav_resistance_system->UpdateCavitation();
	cav_resistance_system->UpdateResistance();

	//Update the waterflow
	ihyd.J  = ihyd.delta_Psi / ihyd.R_total * 86400.0;

	return ihyd.J - J_target;
}


void EvapotranspirationSystem::aerodynamic_conductance_leuning()
{
	// Boundary layer conductance from forced convection (Monteith 1973)
	// according to Leuning 1995, equation E1
	const double g_bHw = 0.003* std::sqrt(windspeed / LEAF_WIDTH);

	// Grashof number according to Leuning 1995, equation E4
	const double grashof = 1.6e8 * fabs(ihyd.temp_leaf - temp_air)* std::pow(LEAF_WIDTH, 3);

	// Boundary layer conductance from free convection (Monteith 1973)
	// according to Leuning 1995, equation E3
	const double g_bHf = 0.5 * DIFF_AIR * std::pow(grashof, 0.25) / LEAF_WIDTH;

	// Total bounday layer conductance to heat for ONE side of a leaf is the sum of
	// free and forced convection, Leuning 1995, equation E5
	double g_bH = g_bHw + g_bHf;

	// We assume that all Pts act as amphistomatous leaves, meaning they can transpire from
	// both sides and thats why we have do double the conductances
	// This may be a Pft parameter one time, if we have hypostomatous leaves..
	// g_bH += g_bH;
	// Cuticular conductance for one side of the leaf m/s.
	g_bH += 0.00012;
	// To get from heat conductance to water vapor conductance we have to multiply by 1.075
	// Leuning 1995, short after equation E5. Our aerodynamic conductance is the always larger
	// than a minimum conductance
	ihyd.ga_run = std::max(MIN_GA, 1.075 * g_bH);
}

void EvapotranspirationSystem::aerodynamic_conductance_ED2()
{
	// equation 10.8
	// Grashof coefficient [1/(K m�)]
	const double GRAS = 3.43e-3 * 9.81 / 1.516e-5 / 1.516e-5;


	/// Parameters for the aerodynamic resistance between the leaf (flat surface) and
	/// the canopy air space.
	/// Monteith, J. L., M. H. Unsworth, 2008. Principles of Environmental Physics,     !
	/// !3rd.edition, Academic Press, Amsterdam, 418pp.  (Mostly Chapter 10)
	const double aflat_lami = 0.6;
	const double nflat_lami = 0.5;
	const double aflat_turb = 0.032;
	const double nflat_turb = 0.8;
	const double bflat_lami = 0.5;
	const double mflat_lami = 0.25;
	const double bflat_turb = 0.13;
	const double mflat_turb = 1.0 / 3.0;


	// Nusselt number calculations based on  ED2 case (1) (i_blyr_condct)
	const double beta_r1 = 7.0 / 4.0;
	const double beta_r2 = 3.0 / 4.0;
	const double beta_re0 = 2000.;
	const double beta_g1 = 3.0 / 2.0;
	const double beta_g2 = -1.0 / 2.0;
	const double beta_gr0 = 100000.;


	// I came up with this as minimum aerodynamic conductance [m/s]
	double gbhmos_min = 0.001;


	// Compute the Reynolds number
	double reynolds = windspeed * LEAF_WIDTH / DIFF_AIR;
	// Compute the Nusselt number for both the laminar and turbulent case.
	double nusselt_lami = aflat_lami * std::pow(reynolds, nflat_lami);
	double nusselt_turb = aflat_turb * std::pow(reynolds, nflat_turb);
	// Compute the correction term for the theoretical Nusselt numbers
	double beta_forced = beta_r1 + beta_r2 * std::tanh(std::log(reynolds / beta_re0));
	// The right Nusselt number is the largest
	double nusselt = beta_forced * std::max(nusselt_lami, nusselt_turb);
	// The conductance is given by MU08 - equation 10.4
	double forced_gbh_mos = DIFF_AIR * nusselt / LEAF_WIDTH;


	/// Leuning 1995 equation E4;
	double grashof = GRAS* fabs(temp_air - ihyd.temp_leaf)* LEAF_WIDTH* LEAF_WIDTH*LEAF_WIDTH;

	nusselt_lami = bflat_lami * std::pow(reynolds, mflat_lami);
	nusselt_turb = bflat_turb * std::pow(reynolds, mflat_turb);

	double beta_free = negligible(grashof) ? beta_g1 - beta_g2 : beta_g1 + beta_g2 * std::tanh(std::log(grashof / beta_gr0));

	nusselt = beta_free * std::max(nusselt_lami, nusselt_turb);

	double free_gbh_mos = DIFF_AIR * nusselt / LEAF_WIDTH;

	ihyd.ga_run = std::max(gbhmos_min, 1.075*(free_gbh_mos + forced_gbh_mos));
}

void EvapotranspirationSystem::aerodynamic_conductance_unknown()
{
	// Hight of airspeed measured, lets assume 30 meters...
	const double z = 30;

	// Atmospheric pressure assumend to be 100 kPa or 1 bar for now
	const double pa = 100.0;

	// Molar densitiy in mol/m^3
	const double rho_hat = 44.6 * pa / 101.3 * K2degC / (temp_air + K2degC);


	// Roughness parameters in m
	const double d = 0.65 * indiv.height;
	const double zm = 0.1* indiv.height;
	const double zh = 0.02* indiv.height;

	// Aerodynamic conductance in mol/m^2/s
	double ga_mol = 0.4*0.4 * rho_hat*windspeed / (std::log((z - d) / zm)*std::log((z - d) / zh));

	ihyd.ga_run = ga_mol / rho_hat;
}
