//////////////////////////////////////////////////////////////////////////////////////
/// \file plant_hydraulic_systems.h
/// \brief Interface for Hydro Dynamics
/// Hydro dynamics (different hydraulic architecures
/// are implemented via a Strategy Design Pattern (SDP)
///
/// \author Phillip Papastefanou
/// $Date: 2017-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "guessmath.h"
#include <cstddef>
#include "plant_hydraulic_processes.h"
#include <memory>


const size_t HYDRAULICS_MAJOR = 1;
const size_t HYDRAULICS_MINOR = 10;
const size_t HYDRAULICS_REVISION = 2;
const size_t HYDRAULICS_BUILD = 0;


class Individual;
class Pft;
class Patch;
class Climate;
class Soil;


class IHydroDynamics
{


public:
	IHydroDynamics(Individual& indiv);
	virtual ~IHydroDynamics();


	/*----------------------------------------------------------------------*/
	/* Main members															*/
	/*----------------------------------------------------------------------*/

	/// Total plant transiration on fpc basis [mm / s]
	virtual double get_et_plant_total() = 0;

	/// Big leaf / canopy conductance for photosynthesis [mm / s]
	virtual double get_gC() = 0;

	// Canopy conductance (leaf conductance LAI weighted) [m / s]
	virtual double get_gC_water() = 0;

	/// Is this day's canopy condutance is water stress
	virtual bool get_is_water_Stressed_GC() = 0;

	/// Is this day's canopy condutance is water stress
	virtual double get_hydraulic_failure_mortality() = 0;

	virtual double get_waterFlow() = 0;

	/*----------------------------------------------------------------------*/
	/* Fields																*/
	/*----------------------------------------------------------------------*/

	/// Soil viscosity [-]
	double eta_soil;
	/// Air viscostity [-]
	double eta_air ;
	/// Leaf area [m2]
	double A_L;
	/// Sapwood area [m2]
	double A_s;

	/*----------------------------------------------------------------------*/
	/* Member functions														*/
	/*----------------------------------------------------------------------*/

	/// Update Hydro Dynamics on a daily basis with timestep = 1day.
	virtual void Update() = 0;

protected:
	/*----------------------------------------------------------------------*/
	/* References															*/
	/*----------------------------------------------------------------------*/

	Patch& patch;
	Individual& indiv;
	PlantHydraulics& ihyd;
	Pft& pft;
	Soil& soil;


	/*----------------------------------------------------------------------*/
	/* Driver References													*/
	/*----------------------------------------------------------------------*/

	/// Todays air temperature reference [�C]
	const double& temp_air;

	/// Todays daylength reference [h]
	const double& daylength;

	/// Todays vapor pressure deficit reference [MPa]
	const double& vpd;

	/*----------------------------------------------------------------------*/
	/* Constants															*/
	/*----------------------------------------------------------------------*/


	/*----------------------------------------------------------------------*/
	/* Member Functions														*/
	/*----------------------------------------------------------------------*/

	void UpdateDerivedVars();

private:

	/*----------------------------------------------------------------------*/
	/* Member Functions														*/
	/*----------------------------------------------------------------------*/

	void soil_air_viscosity();
	void sapwood_area();
};


class HydroDynamicsVPD : public	IHydroDynamics
{
public:
	HydroDynamicsVPD(Individual& indiv);

	~HydroDynamicsVPD() override;
	void Update() override;

	double get_hydraulic_failure_mortality() override;
	double get_et_plant_total() override;
	double get_gC() override;
	double get_gC_water() override;
	bool get_is_water_Stressed_GC() override;
	double get_waterFlow() override;
private:

	std::unique_ptr<EvapotranspirationSystem> evapotranspiration_system;
	std::unique_ptr<PsiSoilLeafSystem> psi_soil_leaf_system;
	std::unique_ptr<CavitationResistanceSystem>cav_resistance_system;
	std::unique_ptr<ReversePsiLeafSystem> reverse_psi_leaf_system;

	WeibullCDF mortality_distribution;
};

class HydroDynamicsEmpty: public IHydroDynamics
{
public:
	HydroDynamicsEmpty(Individual& indiv);
	~HydroDynamicsEmpty();
	void Update() override;

	double get_gC() override;
	double get_gC_water() override;
	double get_et_plant_total() override;
	bool get_is_water_Stressed_GC() override;
	double get_hydraulic_failure_mortality() override;
	double get_waterFlow() override;
};

