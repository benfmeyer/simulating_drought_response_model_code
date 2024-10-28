#include "plant_hydraulic_systems.h"
#include "guess.h"

IHydroDynamics::IHydroDynamics(Individual& indiv) : eta_soil(0.0), eta_air(0.0), A_L(0.0),
													A_s(0.0),
													indiv(indiv),
													pft(indiv.pft),
													patch(indiv.vegetation.patch),
													ihyd(indiv.hydraulic_data),
													temp_air(patch.get_climate().temp),
													soil(patch.soil),
													vpd(patch.get_climate().vpd),
													daylength(patch.get_climate().daylength)
{
}

IHydroDynamics::~IHydroDynamics()
{
}


void IHydroDynamics::UpdateDerivedVars()
{
	soil_air_viscosity();
	sapwood_area();
}

void IHydroDynamics::soil_air_viscosity()
{
	// Based on Hickler et al. 2006
	eta_air = 0.556 + 0.022 * temp_air;
	eta_soil = 0.556 + 0.022 * soil.get_soil_temp_25();

	// Fix to avoid negative viscosities
	if(temp_air < 0.0){
		eta_air = 0.556;
	}

	if(soil.get_soil_temp_25() < 0.0){
		eta_soil = 0.556;
	}
}

void IHydroDynamics::sapwood_area()
{
	// Adapted from Hickler et al. 2006
	// Todo Remove
	A_s = indiv.cmass_sap / pft.wooddens / indiv.height;
}

HydroDynamicsVPD::HydroDynamicsVPD(Individual& indiv):
	IHydroDynamics(indiv), mortality_distribution(0.85, 8.0)
{

	if (param["file_vpd"].str == "" && param["file_relhum"].str == "" && param["file_pres"].str == "" && param["file_specifichum"].str == ""){
		fail("Hydraulics-Error: No (1)VPD or (2)relative humidity, or (3)specific humidity and pressure input driver found. One of the three needs to be specified!");
	}

	if (param["file_wind"].str == "")
		fail("Hydraulics-Error: No windspeed input driver found.");



	// Create Hydraulic sub systems
	psi_soil_leaf_system = std::make_unique<PsiSoilLeafSystem>(indiv, *this);
	cav_resistance_system = std::make_unique<CavitationResistanceSystem>(indiv, *this);
	evapotranspiration_system = std::make_unique<EvapotranspirationSystem>(indiv, *this);
	reverse_psi_leaf_system = std::make_unique<ReversePsiLeafSystem>();


	// Connect hydraulics subsystems together
	cav_resistance_system->ConnectTo(psi_soil_leaf_system.get());
	evapotranspiration_system->ConnectTo(psi_soil_leaf_system.get());
	evapotranspiration_system->ConnectTo(cav_resistance_system.get());
	reverse_psi_leaf_system->ConnectTo(psi_soil_leaf_system.get());
	reverse_psi_leaf_system->ConnectTo(cav_resistance_system.get());
	reverse_psi_leaf_system->ConnectTo(evapotranspiration_system.get());
	//evapotranspiration_system->ConnectTo(reverse_psi_leaf_system.get());

}

HydroDynamicsVPD::~HydroDynamicsVPD()
{
}

void HydroDynamicsVPD::Update()
{
	// Main hydraulic routine
	// Step zero calculates viscosities and leaf areas (if changed)
	UpdateDerivedVars();

	// Soil water potential is estiamted based on soil water content
	psi_soil_leaf_system->UpdatePsiSoil();

	// Based on soil water potential leaf water potential is estimated using Papastefanou et al. 2020
	psi_soil_leaf_system->update_water_potentials(psi_soil_leaf_system->InternalPsiLeaf());

	// Cavitation/Loss-of-hydraulic conductances is esimated based on leaf, xylem and soilwater pot
	cav_resistance_system->UpdateCavitation();

	// Resistances of leaves, roots and xylems is esimated based on the loss-of-conductivities
	cav_resistance_system->UpdateResistance();

	// Canopy conductivity is esimated based on VPD, psi_soil, psi_leaf and the Resistances
	evapotranspiration_system->UpdateConductivity();

	// Do we have more water than the plant needs for full photosynthesis?
	// If yes --> reduce gc
	evapotranspiration_system->CheckForAtmosphericReduction();

	// Calculate today's transpiration
	evapotranspiration_system->UpdateTranspiration();

}

double HydroDynamicsVPD::get_et_plant_total()
{
	return ihyd.et_plant_total;
}

double HydroDynamicsVPD::get_gC()
{
	return ihyd.gC;
}

double HydroDynamicsVPD::get_gC_water()
{
	return ihyd.gC_water;
}

bool HydroDynamicsVPD::get_is_water_Stressed_GC()
{
	return ihyd.waterStressedGC;
}

double HydroDynamicsVPD::get_hydraulic_failure_mortality() {
	return mortality_distribution.Get(ihyd.kappa_s_min);
}

double HydroDynamicsVPD::get_waterFlow()
{
	return ihyd.J;
}

//double HydroDynamicsVPD::get_et_plant_eql_frac() const
//{
//	return et_plant_eql() * get_omega() * indiv.fpc;
//}
//
//double HydroDynamicsVPD::get_et_plant_imp_frac() const
//{
//	return et_plant_imp() * (1 - get_omega()) * indiv.fpc;
//}

HydroDynamicsEmpty::HydroDynamicsEmpty(Individual& indiv):IHydroDynamics(indiv)
{

}

HydroDynamicsEmpty::~HydroDynamicsEmpty()
{
}

void HydroDynamicsEmpty::Update()
{
}



double HydroDynamicsEmpty::get_et_plant_total()
{
	return 0.0;
}

double HydroDynamicsEmpty::get_gC()
{
	return 0.0;
}

double HydroDynamicsEmpty::get_gC_water()
{
	return 0.0;
}


bool HydroDynamicsEmpty::get_is_water_Stressed_GC()
{
	return false;
}

double HydroDynamicsEmpty::get_hydraulic_failure_mortality() {
	return false;
}

double HydroDynamicsEmpty::get_waterFlow()
{
	return 0;
}
