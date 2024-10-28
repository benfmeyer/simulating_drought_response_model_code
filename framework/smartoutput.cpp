#include "smartoutput.h"
#ifdef HAVE_SMARTIO
#include "commonoutput.h"
#include "guess.h"
#include <chrono>
#include <typeinfo>
#include "internal_in/NcInputSingle.h"
#include "internal_in/NcInputParallel.h"
#include "ncinput.h"
#include "internal_in/NcInputParallelAutoSingle.h"


// Nitrogen output is in kgN/ha instead of kgC/m2 as for carbon
//const double M2TOHA = 10000.0;


namespace GuessOutput
{
	REGISTER_OUTPUT_MODULE("SmartOut", SmartOutput)

	SmartOutput::SmartOutput() :
	SmartOutputBuilder(so_options),
		active_fraction_(0.0),
		defined_dims_(false),
		selected_standpft_(nullptr),
		selected_patchpft_(nullptr)
	{

	}


	SmartOutput::~SmartOutput()
	{
		// Only important for the parallel runs, typically buffered memory is freed here
		for (std::reference_wrapper<BaseWriter> writer : writers)
			writer.get().Finalize();

		// Closing of the netcdf files...
		for (std::reference_wrapper<BaseWriter> writer : writers)
			writer.get().Close();
	}


	// Writers will only be added if they are not supressed in the options
	void SmartOutput::CheckWritersForSupression()
	{
		if (!opts.supress_annualy_output)	
			writers.push_back(*awriter);
		if (!opts.supress_monthly_output)
			writers.push_back(*mwriter);
		if (!opts.supress_daily_output)
			writers.push_back(*dwriter);

		if (writers.empty())
			fail("No SmartOutput writers specified, enable at least one writer or disable SmartOutput module");
	}


	void SmartOutput::init(InputModule* input_module, const char* output_directory, const bool verbose)
	{
		const InputModuleType input_module_type = input_module->GetType();

		opts.verbose = verbose;
		opts.input_module_type = input_module_type;
	
		try
		{
			if (input_module_type ==  InputModuleType::NCS){
				awriter = std::make_unique<SingleWriterAnnualy>(opts, output_directory);
				mwriter = std::make_unique<SingleWriterMonthly>(opts, output_directory);
				dwriter = std::make_unique<SingleWriterDaily>(opts, output_directory);

				NcInputSingle* ncs_input = dynamic_cast<NcInputSingle*>(input_module);							
				input_ms = &ncs_input->GetInputMs();			
			}			

			else if (input_module_type == InputModuleType::NCP){
				#ifdef HAVE_PNETCDF
				awriter = std::make_unique<nc_guess_interal::ParallelWriterAnnualy>(opts, output_directory);
				mwriter = std::make_unique<nc_guess_interal::ParallelWriterMonthly>(opts, output_directory);
				dwriter = std::make_unique<nc_guess_interal::ParallelWriterDaily>(opts, output_directory);

				NcInputParallel* ncp_input = dynamic_cast<NcInputParallel*>(input_module);
				input_ms = &ncp_input->GetInputMs();
				#else 
				fail("No valid PNetCDF implementation found");
				#endif
			}

			else if (input_module_type == InputModuleType::NCPA){
				#ifdef HAVE_PNETCDF
				awriter = std::make_unique<nc_guess_interal::ParallelWriterAnnualy>(opts, output_directory);
				mwriter = std::make_unique<nc_guess_interal::ParallelWriterMonthly>(opts, output_directory);
				dwriter = std::make_unique<nc_guess_interal::ParallelWriterDaily>(opts, output_directory);

				NcInputParallelAutoGuess* ncp_input = dynamic_cast<NcInputParallelAutoGuess*>(input_module);
				input_ms = &ncp_input->GetInputMs();
				#else 
				fail("No valid PNetCDF implementation found");
				#endif
			}
					   
			else if (input_module_type == InputModuleType::NCPS){
			
				awriter = std::make_unique<SingleWriterAnnualy>(opts, output_directory);
				mwriter = std::make_unique<SingleWriterMonthly>(opts, output_directory);
				dwriter = std::make_unique<SingleWriterDaily>(opts, output_directory);

				NcInputParallelAutoSingleGuess* ncps_input = dynamic_cast<NcInputParallelAutoSingleGuess*>(input_module);
				input_ms = &ncps_input->GetInputMs();
		
				sender_ = ncps_input->Get_MPI_Input();
			}


			else{
				fail("SmartOutput only works with ncs, ncp, ncpa or ncps input module. Please set \"USE_SMARTIO\" to OFF in CMakeLists.txt and (1) run CMake and (2) recompile LPJ-GUESS again");
			}

			CheckWritersForSupression();
			
			using namespace out_guess;


			/// Initialse nc_guess writers
			for (std::reference_wrapper<BaseWriter> writer : writers)
				writer.get().Init();

			/// Dßefine the base out_guess-variables
			Define();

			for (std::reference_wrapper<BaseWriter> writer : writers)
				writer.get().ConnectToPatchCollector(&stand_patch_collectors_);

			for (std::reference_wrapper<BaseWriter> writer : writers)
				writer.get().ConnectToPftCollector(&stand_patch_pft_collectors_);
		}

		catch (NcGuessException& ex)
		{
			dprintf("NcGuessException thrown: \n");
			fail(ex.what());
		}
		catch (...)
		{
			dprintf("Exeption of unhandled error thrown: \n");
		}


		nindivis_alive_stand_pft.resize(npft);

		//Start the timer fo the simulations
		start = std::chrono::high_resolution_clock::now();

	}

	void SmartOutput::outannual(Gridcell& gridcell)
	{
		
		try
		{
			
		// Dims can only be defined as late as this point, as in the current implementation of LPJ-GUESS
		// we get information about simulation years and gridcells only the Gridcell...
		// However, this has do be done only once per file
		if (!defined_dims_)
		{
			for (std::reference_wrapper<BaseWriter> writer : writers)
			{
				writer.get().SetTimeBeginEnd(gridcell.get_first_year(), gridcell.get_last_year());
				writer.get().SetProcesses(gridcell.get_n_proc_gridcells(), gridcell.get_n_total_gridcells());
				writer.get().SetTimeSteps();
				writer.get().DefineDims();
				writer.get().DefineBaseVariables();
				writer.get().DefineVariables();
				writer.get().WriteTime();
				writer.get().WriteStands();
				writer.get().WritePfts();
			}
			defined_dims_ = true;
		}

		// Accuire data for each writer 
		for (std::reference_wrapper<BaseWriter> writer : writers)
			writer.get().AccuireData();

		// All data will be only written at the end of time of each gridcell simulation
		if (date.get_calendar_year() == writers[0].get().YearEnd())
		{
			// End of this gridcells simulation, measure time
			auto end = std::chrono::high_resolution_clock::now();
			elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			// Reset timer to measure writing speed
			//start = std::chrono::high_resolution_clock::now();
			for (std::reference_wrapper<BaseWriter> writer : writers)
			{
				writer.get().SetStationIndexAndID(gridcell.get_index(), gridcell.get_id());
				writer.get().SetCurrentLonLat(gridcell.get_lon(), gridcell.get_lat());
                writer.get().SetCurrentStationName(gridcell.get_description());


				if (opts.input_module_type == InputModuleType::NCPS)
				{
					writer.get().GatherData();
				}

				else
				{
					writer.get().WriteStationID();
					writer.get().WriteLonLat();
                    writer.get().WriteStationName();
					writer.get().WriteExecutionTime(elapsed_ms, *input_ms);
					writer.get().WriteAllVariables();
				}
			}




			int size = 0;

			if (opts.input_module_type == InputModuleType::NCPS)
			{
				std::vector<double> data;

				for (std::reference_wrapper<BaseWriter> writer : writers)
				{
					std::vector<double>& subdata = writer.get().data_;
					data.insert(data.end(), subdata.begin(), subdata.end());			

				}

				size = data.size();
				start = std::chrono::high_resolution_clock::now();
				sender_->SendData(data);
				end = std::chrono::high_resolution_clock::now();


				for (std::reference_wrapper<BaseWriter> writer : writers)
				{
					writer.get().data_.clear();
				}

			}



			for (std::reference_wrapper<BaseWriter> writer : writers)
			{
				writer.get().ClearData();
			}
			
			//end = std::chrono::high_resolution_clock::now();
			writing_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			opts.Print("Writing took " + std::to_string(writing_ms) + " ms. \n");
			opts.Print("NElems " + std::to_string(size) + " ms. \n");


			// Reset timer to measure time of new gridcell
			start = std::chrono::high_resolution_clock::now();
		}

		//At the end of a gridcell simulation data of the patch and patchpft collectors is reseted
		for (int s = 0; s < nst; ++s)
		{
			stand_patch_collectors_[s].ClearStand();
			stand_patch_pft_collectors_[s].ClearStand();
		}

		}

		catch (NcGuessException& ex)
		{
			dprintf("NcGuessExeption thrown: \n");
			fail(ex.what());
		}
		catch (...)
		{
			dprintf("Exeption of unhandled error thrown: \n");
		}		
	}

	void SmartOutput::outdaily(Gridcell& gridcell)
	{
		size_t year_begin = this->writers[0].get().YearBegin();
		size_t year_end = this->writers[0].get().YearEnd();

		//Only accumulate output for the time periods of interest!
		if (date.get_calendar_year() >= year_begin && date.get_calendar_year() <= year_end)
		{
			PftLoop(gridcell);
		}			
		
	}

	void SmartOutput::openlocalfiles(Gridcell& gridcell)
	{
	}

	void SmartOutput::closelocalfiles(Gridcell& gridcell)
	{
	}


	void SmartOutput::GetActiveFraction(Gridcell& gridcell, Pft& pft)
	{
		// Determine area fraction of stands where this pft is active:
		active_fraction_ = 0.0;
		Gridcell::iterator gc_itr = gridcell.begin();
		while (gc_itr != gridcell.end()) {
			Stand& stand = *gc_itr;

			if (stand.pft[pft.id].active) {
				active_fraction_ += stand.get_gridcell_fraction();
			}
			++gc_itr;
		}
	}

	void SmartOutput::PftLoop(Gridcell& gridcell)
	{

		// Get active fraction for pft
		pftlist.firstobj();
		while (pftlist.isobj)
		{
			Pft& pft = pftlist.getobj();
			GetActiveFraction(gridcell, pft);
			pftlist.nextobj();
		}

		// Receive number of all active individuals across all patches
		// This is necessary to be done before iterating through the patches
		// because if we want to calculate a mean value overall individuals we cannot
		// simply count all the indivs per patch and divide with npatch
		// If a patch does not contain any active indivs we get a wrong output by dividing with npatch


		
		// Call stand loop, no actual loop 
		StandLoop(gridcell);

	}

	void SmartOutput::StandLoop(Gridcell& gridcell)
	{
		try
		{
			// Loop through Stands
			Gridcell::iterator gc_itr = gridcell.begin();
			while (gc_itr != gridcell.end())
			{
				Stand& stand = *gc_itr;
				int stid = stand.id;

				//Currently not used
				//selected_standpft_ = &stand.pft[pft.id];

				// Count all individuals of this stand to account 
				// for the correct mean value of an individual
				CountAliveIndiviualsAcrossAllPatches(stand);


				// Loop through patches
				PatchLoop(stand);

				//At the last day of year...
				if (date.islastday && date.islastmonth)
				{

					// Divide by active fraction
					stand_patch_pft_collectors_[stid].RescalePatch(stand.get_gridcell_fraction() / active_fraction_);
					stand_patch_collectors_[stid].RescalePatch(stand.get_gridcell_fraction() / active_fraction_);

					// Divide/Rescale on temporal basis, e.g. is a value accumulated over the year or just the mean taken
					stand_patch_pft_collectors_[stid].TimeRescalePatch();
					stand_patch_collectors_[stid].TimeRescalePatch();

					// Transfer patch values on stand level
					stand_patch_pft_collectors_[stid].AccValPerStand();
					stand_patch_collectors_[stid].AccValPerStand();

					// Reset patch values, so they can be accumulated for the next year
					stand_patch_pft_collectors_[stid].ClearPatch();
					stand_patch_collectors_[stid].ClearPatch();
				}

				++gc_itr;
			}
		}
		catch (OutGuessException& ex)
		{
			dprintf("OutGuessException thrown: \n");
			fail(ex.what());
		}

	}

	void SmartOutput::PatchLoop(Stand& stand)
	{
		// Loop through Patches
		stand.firstobj();
		while (stand.isobj)
		{
			Patch& patch = stand.getobj();

			// Main individual loop
			IndivLoop(stand, patch);			

			//Set value for each Patch-Variable
			SetPatchValue(patch);


			//Todo improve this section
			if (date.day == 364)
			{
				for (int i = 0; i < npft; i++)
				{
					const Patchpft& ppft = patch.pft[i];
					const double npatch_d = npatch;

					cmass_lost_bg.AccPftValue(ppft.cmass_lost_bg / npatch_d, ppft.id, date);
					cmass_lost_greff.AccPftValue(ppft.cmass_lost_greff / npatch_d, ppft.id, date);
					cmass_lost_cav.AccPftValue(ppft.cmass_lost_cav / npatch_d, ppft.id, date);

					cmass_lost_leaf_bg.AccPftValue(ppft.cmass_lost_leaf_bg / npatch_d, ppft.id, date);
					cmass_lost_leaf_greff.AccPftValue(ppft.cmass_lost_leaf_greff / npatch_d, ppft.id, date);
					cmass_lost_leaf_cav.AccPftValue(ppft.cmass_lost_leaf_cav / npatch_d, ppft.id, date);

					cmass_lost_sap_bg.AccPftValue(ppft.cmass_lost_sap_bg / npatch_d, ppft.id, date);
					cmass_lost_sap_greff.AccPftValue(ppft.cmass_lost_sap_greff / npatch_d, ppft.id, date);
					cmass_lost_sap_cav.AccPftValue(ppft.cmass_lost_sap_cav / npatch_d, ppft.id, date);

					cmass_lost_heart_bg.AccPftValue(ppft.cmass_lost_heart_bg / npatch_d, ppft.id, date);
					cmass_lost_heart_greff.AccPftValue(ppft.cmass_lost_heart_greff / npatch_d, ppft.id, date);
					cmass_lost_heart_cav.AccPftValue(ppft.cmass_lost_heart_cav / npatch_d, ppft.id, date);

					cmass_lost_root_bg.AccPftValue(ppft.cmass_lost_root_bg / npatch_d, ppft.id, date);
					cmass_lost_root_greff.AccPftValue(ppft.cmass_lost_root_greff / npatch_d, ppft.id, date);
					cmass_lost_root_cav.AccPftValue(ppft.cmass_lost_root_cav / npatch_d, ppft.id, date);

					bminc.AccPftValue(ppft.bminc / npatch_d, ppft.id, date);
					cmass_repr.AccPftValue(ppft.cmass_repr / npatch_d, ppft.id, date);
					npp_leaf.AccPftValue(ppft.npp_leaf / npatch_d, ppft.id, date);
					npp_root.AccPftValue(ppft.npp_root / npatch_d, ppft.id, date);
					npp_sap.AccPftValue(ppft.npp_sap / npatch_d, ppft.id, date);
					npp_heart.AccPftValue(ppft.npp_heart / npatch_d, ppft.id, date);
					npp_debt.AccPftValue(ppft.npp_debt / npatch_d, ppft.id, date);
					litter_root_inc.AccPftValue(ppft.litter_root_inc / npatch_d, ppft.id, date);
					litter_leaf_inc.AccPftValue(ppft.litter_leaf_inc / npatch_d, ppft.id, date);
					cmass_litter.AccPftValue((ppft.litter_leaf + ppft.litter_root + ppft.litter_sap + ppft.litter_heart + ppft.litter_repr) / npatch_d, ppft.id, date);

					fire_per_pft.AccPftValue(patch.fluxes.get_annual_flux(Fluxes::INDIV_FIREC, ppft.id) / npatch_d, ppft.id, date);
				}


			}


			stand.nextobj();
		}		
		
	}

	void SmartOutput::IndivLoop(Stand& stand, Patch& patch)
	{
		Vegetation& vegetation = patch.vegetation;

		//Loop through individuals
		vegetation.firstobj();
		while (vegetation.isobj)
		{
			Individual& indiv = vegetation.getobj();

			//Check if individual is alive
			if (indiv.id != -1 && indiv.alive)
			{
				//Set value for each PatchPft-Variable
				SetPftValue(indiv, patch, indiv.pft);
			}
			
			vegetation.nextobj();
		}
	}

	void SmartOutput::CountAliveIndividuals(Patch& patch)
	{
		
		// Reset counts of pfts
		for (int& count : patch.n_indivs_alive_pft)
			count = 0;
		
		
		Vegetation& vegetation = patch.vegetation;
				
		// Set number of individuals to total number of individuals
		patch.n_indivs_alive = patch.vegetation.nobj;

		// Loop through individuals
		vegetation.firstobj();

		while (vegetation.isobj)
		{
			Individual& indiv = vegetation.getobj();
			const Pft& pft = indiv.pft;

			//Reduce number of individuals if indivis are not alive
			if (indiv.id == -1 || !indiv.alive)
			{
				patch.n_indivs_alive--;
			}

			else
			{
				patch.n_indivs_alive_pft[pft.id]++;
			}
			
			vegetation.nextobj();
		}

	}

	void SmartOutput::CountAliveIndiviualsAcrossAllPatches(Stand& stand)
	{
		// We have to count the number of alive individuals 
		// because sometimes they are not alive but in the vegetation
		nindivis_alive_stand = 0;
		for (int p = 0; p < npft; ++p)
			nindivis_alive_stand_pft[p] = 0;

		
		// Loop through Patches
		stand.firstobj();
		while (stand.isobj)
		{
			Patch& patch = stand.getobj();

			CountAliveIndividuals(patch);

			nindivis_alive_stand += patch.n_indivs_alive;

			for (int p = 0; p < npft; ++p)
				nindivis_alive_stand_pft[p] += patch.n_indivs_alive_pft[p];

			stand.nextobj();
		}

	}

}

#endif
