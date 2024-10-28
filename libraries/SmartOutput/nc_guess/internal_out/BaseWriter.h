///////////////////////////////////////////////////////////////////////////////////////
/// \file BaseWriter.h
/// \brief Main class responsible for writing output
///  This class takes care of both single and parallel output writing
/// \author Phillip Papastefanou
/// $Date: 2017-02-11 23:17:02 +0100 (Sun, 11 Feb 2017) $
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Dimension.h"
#include <vector>
#include <memory>
#include "PatchCollector.h"
#include "PatchPftCollector.h"
#include "NcGuessVar.h"
#include "Writer.h"

#include "lib/SmartOutputBuilderOptions.h"
#include <functional>
//#include "internal_in/MPI_Gridlist_Data_Send.h"
#include "internal_in/MPI_Gridlist_Auto.h"

#ifdef HAVE_PNETCDF
#include "WriterPNC.h"
#else 
#include "WriterNC.h"
#endif

//namespace nc_guess_interal {
//	class MPI_Gridlist_Data_Send;
//}

class InputModule;

class BaseWriter
{
public:
	/**
	 * \brief Abstract Basewriter class which is only used for polymorphism
	 * \param opts options for the smartoutput
	 * \param output_directory the directory where to create the output
	 */
	BaseWriter(SmartOutputBuilderOptions& opts, const std::string output_directory);
	virtual ~BaseWriter();

	/**
	 * \brief Should be called by the base class, Creates files and writes general attributes to files 
	 */
	virtual void Init();

	/**
	 * \brief Connects writer to the PatchCollector
	 * \param patch_collector 
	 */
	void ConnectToPatchCollector(std::vector<out_guess::PatchCollector>* stand_patch_collector);
	/**
	 * \brief Connects writer to the PatchPftCollector
	 * \param patch_collector 
	 */
	void ConnectToPftCollector(std::vector<out_guess::PatchPftCollector>* stand_patch_pft_collector);

	/**
	 * \brief Set the number of gridcells per process and in total the two values should be identical if we are in single mode
	 * \param n_process_gridcells 
	 * \param n_total_gridcells 
	 */
	void SetProcesses(const size_t n_process_gridcells, const size_t n_total_gridcells);
	/**
	 * \brief Set the first year of the output and the last year (inclusive!) 
	 * \param year_begin 
	 * \param year_end 
	 */
	void SetTimeBeginEnd(int year_begin, int year_end);
	/**
	 * \brief Set index and id of the current gridcell.
	 * \param index 
	 * \param id 
	 */
	void SetStationIndexAndID(size_t index, size_t id);

	/**
	 * \brief Sets (lon,lat) for the current gridcell
	 * \param lon 
	 * \param lat 
	 */
	void SetCurrentLonLat(double lon, double lat);
    void SetCurrentStationName(std::string description);
	virtual void SetTimeSteps() = 0;

	/// Define writers dimensions, these are for all writers the same expect the number of timesteps
	void DefineDims();
	/// Defining the variables according to the dimensions
	void DefineBaseVariables();
	/// Define all the ouput variables
	void DefineVariables();
	/// End define mode
	void EndDef() const;
	/// Write the timesteps for each variable, should only be done once per writer
	void WriteTime();
	/// Write the station id of the current cell, should done once per gridcell
	void WriteStationID();
    void WriteStationName();
	/// Write the name of the Pfts, should only be done once per writer
	void WritePfts();
	/// Write the name of the Stands, should only be done once per writer
	void WriteStands();
	/// Write longitude and latitude, should also only done once per gricell
	void WriteLonLat();
	/// Write execution time, should only be called oonce per gridcell and at the end
	void WriteExecutionTime(double elapsed_ms, double input_ms);
	/// Write data of all non basic variables, should be done once per gridcell (at the end of the last simulation year)
	void WriteAllVariables();
	/// When is the data of the collectors transfered to the nc variables 
	void AccuireData();
	/// free allocated data of the NetCDF variables
	void ClearData();
	/// Closing the NetCDF files
	void Close() const;
	/// Only affects the parallel writers Todo check wheter this should be called before close!
	void Finalize();


	size_t NGridcells() const;
	size_t NTimeSteps() const;
	size_t CurrentStationID() const;
	size_t CurrentStationIndex() const;

	int YearBegin() const;
	int YearEnd() const;


	void WriteAllVariablesSingle(double* data, int& offset);
	void GatherData();
	//void ConnectToMPIModule(nc_guess_interal::MPI_Gridlist_Data_Send* sender);
	//nc_guess_interal::MPI_Gridlist_Data_Send* mpi_gather_module_;

	std::vector<double> data_;


protected:
	InputModuleType input_module_type;
	string filename_;
	string output_directory_;
	WriterType writer_type_;
	OutWriterType out_writer_type_;
	DataFormat& data_format_;


	std::unique_ptr<Writer> writer_;
	#ifdef HAVE_PNETCDF
	WriterPNC* writerPNC_;
	#else
	WriterNC* writerNC_;
	#endif

	vector<std::shared_ptr<NcGuessVar<double>>> base_variables_d_;
	//std::shared_ptr<out_guess::PatchCollector> patch_collector_;
	//std::shared_ptr<out_guess::PatchPftCollector> patch_pft_collector_;

	std::vector<out_guess::PatchCollector>*  stand_patch_collectors_;
	std::vector<out_guess::PatchPftCollector>* stand_patch_pft_collectors_;

	SmartOutputBuilderOptions& opts_;

	vector<Dimension> dimensions_;

	bool wrote_time_;
	bool wrote_pfts_;

	int year_begin_;
	int year_end_;

	Group maingroup_;
	Group patchgroup_;
	Group patchpftgroup_;

	size_t n_years_;
	size_t n_timesteps_;
	size_t time_res_;
	size_t n_proc_gridcells_;
	size_t n_total_gridcells_;

	size_t n_ncvars_;
	vector<long> bufsizes_float_;

	string time_unit_;

	size_t current_station_id_;
	size_t current_station_index_;
    std::string current_station_name_;

	double current_lon_;
	double current_lat_;

	void CountNcVars(const out_guess::OutputVarPatch* out_var);
	void CountNcVars(const out_guess::OutputVarPft* out_var);
	void Print(const string& message) const;

private:
	void AddPftAttributes(Variable& var) const;
};
