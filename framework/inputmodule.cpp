///////////////////////////////////////////////////////////////////////////////////////
/// \file inputmodule.cpp
/// \brief Implemenation file for inputmodule.h
///
/// \author Joe Siltberg
/// $Date: 2016-12-08 18:24:04 +0100 (Thu, 08 Dec 2016) $
///
///////////////////////////////////////////////////////////////////////////////////////
#include "config.h"
#include "inputmodule.h"
#include "guess.h"
#include "demoinput.h"
#include "cruinput.h"
#include "cfinput.h"
#ifdef HAVE_SMARTIO
#include "internal_in/NcInputSingle.h"
#include "internal_in/NcInputParallelAutoSingle.h"
#include "internal_in/NcInputParallel.h"
#include "internal_in/NcInputParallelAuto.h"
#endif 
#include "ncinput.h"

InputModuleType InputModule::GetType()
{
	return type;
}

std::unique_ptr<InputModule> InputModuleRegistry2::CreateInstance(InputModuleType type)
{
	if (type == InputModuleType::DEMO)
		return std::make_unique<DemoInput>(type);
	else if (type == InputModuleType::CRU_NCEP)
		return std::make_unique<CRUInput>(type);
#ifdef HAVE_NETCDF
	else if (type == InputModuleType::CF)
		return std::make_unique<CFInput>(type);
#endif
#ifdef HAVE_SMARTIO
	else if (type == InputModuleType::NCS)
		return std::make_unique<NcInputSingle>(type);
	else if (type == InputModuleType::NCPS)
		return std::make_unique<NcInputParallelAutoSingleGuess>(type);
	else if (type == InputModuleType::NCP)
		return std::make_unique<NcInputParallel>(type);
	else if (type == InputModuleType::NCPA)
		return std::make_unique<NcInputParallelAutoGuess>(type);
#endif
	else
	{
		fail("Invalid Input module");
		return std::make_unique<DemoInput>(type);
	}

}

