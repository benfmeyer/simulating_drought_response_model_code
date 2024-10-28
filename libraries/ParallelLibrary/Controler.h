#pragma once
#include "gutil.h"
#include <string>

class Controler
{
public:
	Controler();
	~Controler();

	std::string outputdirectory;
	double coordinates_precision;
	// Output file names ...
	xtring file_cmass, file_anpp, file_agpp, file_fpc, file_aaet, file_dens, file_lai, file_cflux, file_doc, file_cpool, file_clitter, file_runoff;
	xtring file_mnpp, file_mlai, file_mgpp, file_mra, file_maet, file_mpet, file_mevap, file_mrunoff, file_mintercep, file_mrh;
	xtring file_mnee, file_mwcont_upper, file_mwcont_lower;
	xtring file_firert, file_speciesheights;

	// bvoc
	xtring file_aiso, file_miso, file_amon, file_mmon;

	// nitrogen
	xtring file_nmass, file_cton_leaf, file_nsources, file_npool, file_nlitter, file_nuptake, file_vmaxnlim, file_nflux, file_ngases;


	//New output variables by PFT - TP 13.11.15
	xtring file_mbclic, file_mnbioc, file_malloc, file_mfirec, file_mgrowc, file_mharvc, file_mortc, file_mortdc, file_turnrc, file_turnlc;
	xtring file_mminc; //TP 29.04.16
	xtring file_cmass_leaf, file_cmass_root, file_cmass_wood;
	xtring file_mplai01, file_mplai02, file_mplai03, file_mplai04, file_mplai05, file_mplai06, file_mplai07, file_mplai08, file_mplai09, file_mplai10, file_mplai11, file_mplai12;
	xtring file_mpgpp01, file_mpgpp02, file_mpgpp03, file_mpgpp04, file_mpgpp05, file_mpgpp06, file_mpgpp07, file_mpgpp08, file_mpgpp09, file_mpgpp10, file_mpgpp11, file_mpgpp12;
	xtring file_mpnpp01, file_mpnpp02, file_mpnpp03, file_mpnpp04, file_mpnpp05, file_mpnpp06, file_mpnpp07, file_mpnpp08, file_mpnpp09, file_mpnpp10, file_mpnpp11, file_mpnpp12;
	xtring file_mpaet01, file_mpaet02, file_mpaet03, file_mpaet04, file_mpaet05, file_mpaet06, file_mpaet07, file_mpaet08, file_mpaet09, file_mpaet10, file_mpaet11, file_mpaet12;

	//Canopy outputs - TP 10.06.17
	xtring file_tallestmean, file_tallestmax, file_tallestmedian, file_dommean, file_dommax, file_dommedian;
};

