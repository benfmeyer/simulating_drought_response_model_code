//////////////////////////////////////////////////////////////////////////////////////
/// \file plant_hydraulic_processes.h
/// \brief Class that contains the plant hydraulic processes
/// devided into different submodules
/// 
/// \author Phillip Papastefanou
/// $Date: 2017-04-27 18:07:55 +0100 (Fri, 27 Apr 2018) $
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
//#include "externalinput.h"
#include "equation_solvers.h"

class IHydroDynamics;
class Pft;
class Patch;
class Individual;
struct PlantHydraulics;

class HydroDynamicsVPD;

class PsiSoilLeafSystem;
class CavitationResistanceSystem;
class EvapotranspirationSystem;

class SubModule
{
public:
	SubModule();
	virtual ~SubModule();
	virtual void ConnectTo(SubModule* subModule) = 0;
};


class PsiSoilLeafSystem : public SubModule, MidpointImplicit, EulerExplicit
{
public:
	PsiSoilLeafSystem(Individual& indiv, IHydroDynamics& hydro_dynamics);
	~PsiSoilLeafSystem();
	void ConnectTo(SubModule* subModule) override;


	void UpdatePsiSoil();
	void update_water_potentials(const double psi_leaf);
    //Get the leaf water potentias based on the solution of Papastefanou et al. 2021
    //Todo Find a better name for that..
    double InternalPsiLeaf();

private:
	
	/*----------------------------------------------------------------------*/
	/* References															*/
	/*----------------------------------------------------------------------*/

	Patch& patch;
	Individual& indiv;
	Pft& pft;
    PlantHydraulics& ihyd;

	/*----------------------------------------------------------------------*/
	/* Solver functions														*/
	/*----------------------------------------------------------------------*/

	// First order derivate of leaf water potential equation from Papastefanou et al. 2020
	double dpsi_leaf_dt(const double t, const double psi_leaf);

	/// linear psi soil interpolation between last day and today
	/// to increase stepsize of the ODE solver
	double ip_psi_soil(double time) const;

	// First order overwritten from the equation solvers
	double dydx(const double x, const double y) override;

	// Second order overwritten from the equation solvers
	double d2ydx2(const double x, const double y) override;

	/*----------------------------------------------------------------------*/
	/* Members														*/
	/*----------------------------------------------------------------------*/
};

class ReversePsiLeafSystem : public SubModule, BisectionSolver
{
public:
	ReversePsiLeafSystem();
	
	void ConnectTo(SubModule* subModule) override;

	void UpdatePsiLeaf(double gC, double psiL_min);
	
	/*----------------------------------------------------------------------*/
	/* Main States															*/
	/*----------------------------------------------------------------------*/


	/*----------------------------------------------------------------------*/
	/* Variables															*/
	/*----------------------------------------------------------------------*/


protected:
	double f(const double x) override;
private:
	
	/*----------------------------------------------------------------------*/
	/* Hydraulic system references											*/
	/*----------------------------------------------------------------------*/
	PsiSoilLeafSystem* psi_soil_leaf_system_;
	CavitationResistanceSystem* cavitation_resistance_system;
	EvapotranspirationSystem* evapotranspiration_system;

	double gC_match;
	
};

class CavitationResistanceSystem: public SubModule
{
public:
	CavitationResistanceSystem(Individual& indiv, IHydroDynamics& hydro_dynamics);
	virtual ~CavitationResistanceSystem();

	/*----------------------------------------------------------------------*/
	/* Member functions														*/
	/*----------------------------------------------------------------------*/
	
	void UpdateCavitation();

	void UpdateResistance();


	void ConnectTo(SubModule* subModule) override;
private:

	/*----------------------------------------------------------------------*/
	/* Constants															*/
	/*----------------------------------------------------------------------*/
	const double PSI_50_RESCALE = 1.5;

	/*----------------------------------------------------------------------*/
	/* References															*/
	/*----------------------------------------------------------------------*/
	Patch& patch;
	Individual& indiv;
	Pft& pft;
    PlantHydraulics& ihyd;

	/*----------------------------------------------------------------------*/
	/* Driver References													*/
	/*----------------------------------------------------------------------*/


	/*----------------------------------------------------------------------*/
	/* Hydraulic system references											*/
	/*----------------------------------------------------------------------*/
	const PsiSoilLeafSystem* psi_soil_leaf_system;

	/*----------------------------------------------------------------------*/
	/* Member functions 													*/
	/*----------------------------------------------------------------------*/
	

	/*----------------------------------------------------------------------*/
	/* Member fields	 													*/
	/*----------------------------------------------------------------------*/
	HillEquation cav_xylem_calc;
	HillEquation cav_leaf_calc;
	HillEquation cav_root_calc;

	/// Sapwood area [m2]
	const double& A_s;
	/// Air viscosity [-]
	const double& eta_a;
	/// Soil viscosity [-]
	const double& eta_s;
	/// Molar mass of water [g/mol]
	const double MOLAR_MASS_WATER = 18.015281;

};


class EvapotranspirationSystem: public SubModule, BisectionSolver
{
public:
	EvapotranspirationSystem(Individual& indiv, HydroDynamicsVPD& hydro_dynamics);
	~EvapotranspirationSystem();

	void ConnectTo(SubModule* subModule) override;
	
	void UpdateConductivity();

	void CheckForAtmosphericReduction();

	void UpdateTranspiration();

private:
	/*----------------------------------------------------------------------*/
	/* References															*/
	/*----------------------------------------------------------------------*/

	Patch& patch;
	Individual& indiv;
	Pft& pft;
    PlantHydraulics& ihyd;

	/*----------------------------------------------------------------------*/
	/* Driver References													*/
	/*----------------------------------------------------------------------*/

	/// equilibrium evapotranspiration today, deducting interception reference (mm)
	const double& eet_net_veg;

	/// total daily net downward shortwave solar radiation today (J/m2/day)
	const double& rad;

	/// windspeed (m/s)
	const double& windspeed;

	/// Todays air temperature reference [�C]
	const double& temp_air;

	/// Todays daylength reference [h]
	const double& daylength;

	/// Todays vapor pressure deficit reference [kPa]
	const double& vpd;
	
	/*----------------------------------------------------------------------*/
	/* Hydraulic system references											*/
	/*----------------------------------------------------------------------*/

    PsiSoilLeafSystem* psi_soil_leaf_system;
    CavitationResistanceSystem* cav_resistance_system;
	//ReversePsiLeafSystem* reverse_psi_leaf_system;
	
	/*----------------------------------------------------------------------*/
	/* Member functions 													*/
	/*----------------------------------------------------------------------*/

	void aerodynamic_conductance_leuning();
	void aerodynamic_conductance_ED2();
	void aerodynamic_conductance_unknown();


	void transpiration();
	void leaf_temperature();



private:
	/// Leaf temperature of previous step[�K]
	double temp_leaf_prev;

	/// Leaf temperature precision as difference of current and previous iteration[�K]
	double temp_leaf_delta;

    /// Waterflow that is sufficient under to supply total evapotranspiration
    double J_target;

    /// Secant solver function for finding the min psiL
    double f(const double x) override;


};





