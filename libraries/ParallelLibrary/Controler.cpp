#include "Controler.h"
#include "parameters.h"


Controler::Controler()
{
	declare_parameter("outputdirectory", &outputdirectory, 300, "Directory for the output files");
	declare_parameter("coordinates_precision", &coordinates_precision, 0, 10, "Digits after decimal point in coordinates in output");
	// Annual output variables
	declare_parameter("file_cmass", &file_cmass, 300, "C biomass output file");
	declare_parameter("file_anpp", &file_anpp, 300, "Annual NPP output file");
	declare_parameter("file_agpp", &file_agpp, 300, "Annual GPP output file");
	declare_parameter("file_fpc", &file_fpc, 300, "FPC output file");
	declare_parameter("file_aaet", &file_aaet, 300, "Annual AET output file");
	declare_parameter("file_lai", &file_lai, 300, "LAI output file");
	declare_parameter("file_cflux", &file_cflux, 300, "C fluxes output file");
	declare_parameter("file_doc", &file_doc, 300, "DOC output file");
	declare_parameter("file_dens", &file_dens, 300, "Tree density output file");
	declare_parameter("file_cpool", &file_cpool, 300, "Soil C output file");
	declare_parameter("file_clitter", &file_clitter, 300, "Litter C output file");
	declare_parameter("file_runoff", &file_runoff, 300, "Runoff output file");
	declare_parameter("file_firert", &file_firert, 300, "Fire retrun time output file");

	declare_parameter("file_nmass", &file_nmass, 300, "N biomass output file");
	declare_parameter("file_cton_leaf", &file_cton_leaf, 300, "Mean leaf C:N output file");
	declare_parameter("file_nsources", &file_nsources, 300, "Annual nitrogen sources output file");
	declare_parameter("file_npool", &file_npool, 300, "Soil nitrogen output file");
	declare_parameter("file_nlitter", &file_nlitter, 300, "Litter nitrogen output file");
	declare_parameter("file_nuptake", &file_nuptake, 300, "Annual nitrogen uptake output file");
	declare_parameter("file_vmaxnlim", &file_vmaxnlim, 300, "Annual nitrogen limitation on vm output file");
	declare_parameter("file_nflux", &file_nflux, 300, "Annual nitrogen fluxes output file");
	declare_parameter("file_ngases", &file_ngases, 300, "Annual nitrogen gases output file");

	declare_parameter("file_speciesheights", &file_speciesheights, 300, "Mean species heights");

	// Monthly output variables
	declare_parameter("file_mnpp", &file_mnpp, 300, "Monthly NPP output file");
	declare_parameter("file_mlai", &file_mlai, 300, "Monthly LAI output file");
	declare_parameter("file_mgpp", &file_mgpp, 300, "Monthly GPP-LeafResp output file");
	declare_parameter("file_mra", &file_mra, 300, "Monthly autotrophic respiration output file");
	declare_parameter("file_maet", &file_maet, 300, "Monthly AET output file");
	declare_parameter("file_mpet", &file_mpet, 300, "Monthly PET output file");
	declare_parameter("file_mevap", &file_mevap, 300, "Monthly Evap output file");
	declare_parameter("file_mrunoff", &file_mrunoff, 300, "Monthly runoff output file");
	declare_parameter("file_mintercep", &file_mintercep, 300, "Monthly intercep output file");
	declare_parameter("file_mrh", &file_mrh, 300, "Monthly heterotrophic respiration output file");
	declare_parameter("file_mnee", &file_mnee, 300, "Monthly NEE output file");
	declare_parameter("file_mwcont_upper", &file_mwcont_upper, 300, "Monthly wcont_upper output file");
	declare_parameter("file_mwcont_lower", &file_mwcont_lower, 300, "Monthly wcont_lower output file");
	// bvoc
	declare_parameter("file_aiso", &file_aiso, 300, "annual isoprene flux output file");
	declare_parameter("file_miso", &file_miso, 300, "monthly isoprene flux output file");
	declare_parameter("file_amon", &file_amon, 300, "annual monoterpene flux output file");
	declare_parameter("file_mmon", &file_mmon, 300, "monthly monoterpene flux output file");

	//New output variables by PFT - TP 13.11.15
	declare_parameter("file_mbclic", &file_mbclic, 300, "Veg. turnover due to bioclimatic limit mortality output file");
	declare_parameter("file_mnbioc", &file_mnbioc, 300, "Veg. turnover due to negative biomass mortality output file");
	declare_parameter("file_malloc", &file_malloc, 300, "Veg. turnover due to bad allometry mortality output file");
	declare_parameter("file_mfirec", &file_mfirec, 300, "Veg. turnover due to fire mortality output file");
	declare_parameter("file_mgrowc", &file_mgrowc, 300, "Veg. turnover due to growth efficiency mortality output file");
	declare_parameter("file_mminc", &file_mminc, 300, "Veg. turnover due to minimum background mortality output file"); //TP 29.04.16
	declare_parameter("file_mharvc", &file_mharvc, 300, "Veg. turnover due to harvest mortality output file");
	declare_parameter("file_mortc", &file_mortc, 300, "Veg. turnover due to other mortality output file");
	declare_parameter("file_mortdc", &file_mortdc, 300, "Veg. turnover due to background disturbance mortality output file");
	declare_parameter("file_turnrc", &file_turnrc, 300, "Root turnover (phenological) output file");
	declare_parameter("file_turnlc", &file_turnlc, 300, "Leaf turnover (phenological) output file");
	declare_parameter("file_cmass_root", &file_cmass_root, 300, "C root biomass output file");
	declare_parameter("file_cmass_leaf", &file_cmass_leaf, 300, "C leaf biomass output file");
	declare_parameter("file_cmass_wood", &file_cmass_wood, 300, "C wood biomass output file");

	declare_parameter("file_mplai01", &file_mplai01, 300, "Monthly by PFT LAI output file (monthly mean)"); //New output variables by PFT - TP 13.11.15
	declare_parameter("file_mplai02", &file_mplai02, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai03", &file_mplai03, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai04", &file_mplai04, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai05", &file_mplai05, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai06", &file_mplai06, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai07", &file_mplai07, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai08", &file_mplai08, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai09", &file_mplai09, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai10", &file_mplai10, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai11", &file_mplai11, 300, "Monthly by PFT LAI output file (monthly mean)");
	declare_parameter("file_mplai12", &file_mplai12, 300, "Monthly by PFT LAI output file (monthly mean)");

	declare_parameter("file_mpgpp01", &file_mpgpp01, 300, "Monthly by PFT GPP output file"); //New output variables by PFT - TP 13.11.15
	declare_parameter("file_mpgpp02", &file_mpgpp02, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp03", &file_mpgpp03, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp04", &file_mpgpp04, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp05", &file_mpgpp05, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp06", &file_mpgpp06, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp07", &file_mpgpp07, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp08", &file_mpgpp08, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp09", &file_mpgpp09, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp10", &file_mpgpp10, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp11", &file_mpgpp11, 300, "Monthly by PFT GPP output file");
	declare_parameter("file_mpgpp12", &file_mpgpp12, 300, "Monthly by PFT GPP output file");

	declare_parameter("file_mpnpp01", &file_mpnpp01, 300, "Monthly by PFT NPP output file"); //New output variables by PFT - TP 13.11.15
	declare_parameter("file_mpnpp02", &file_mpnpp02, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp03", &file_mpnpp03, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp04", &file_mpnpp04, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp05", &file_mpnpp05, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp06", &file_mpnpp06, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp07", &file_mpnpp07, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp08", &file_mpnpp08, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp09", &file_mpnpp09, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp10", &file_mpnpp10, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp11", &file_mpnpp11, 300, "Monthly by PFT NPP output file");
	declare_parameter("file_mpnpp12", &file_mpnpp12, 300, "Monthly by PFT NPP output file");

	declare_parameter("file_mpaet01", &file_mpaet01, 300, "Monthly by PFT transpiration output file"); //New output variables by PFT - TP 13.11.15
	declare_parameter("file_mpaet02", &file_mpaet02, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet03", &file_mpaet03, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet04", &file_mpaet04, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet05", &file_mpaet05, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet06", &file_mpaet06, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet07", &file_mpaet07, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet08", &file_mpaet08, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet09", &file_mpaet09, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet10", &file_mpaet10, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet11", &file_mpaet11, 300, "Monthly by PFT transpiration output file");
	declare_parameter("file_mpaet12", &file_mpaet12, 300, "Monthly by PFT transpiration output file");

	declare_parameter("file_tallestmean", &file_tallestmean, 300, "Properties of tallest individual in patch (stand mean)"); // Canopy output variables - TP 10.06.17
	declare_parameter("file_tallestmax", &file_tallestmax, 300, "Properties of tallest individual in patch (stand max)");
	declare_parameter("file_tallestmedian", &file_tallestmedian, 300, "Properties of tallest individual in patch (stand median)");
	declare_parameter("file_dommean", &file_dommean, 300, "Properties of dominant individual in patch (stand mean)");
	declare_parameter("file_dommax", &file_dommax, 300, "Properties of dominant individual in patch (stand max)");
	declare_parameter("file_dommedian", &file_dommedian, 300, "Properties of dominant individual in patch (stand median)");


}


Controler::~Controler()
{
}
