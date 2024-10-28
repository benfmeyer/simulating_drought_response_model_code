	#add_subdirectory(out_guess)
	#include_directories(out_guess)	

	#add_subdirectory(nc_guess)
	#include_directories(nc_guess)

	#add_subdirectory(nc_base)
	include_directories(libraries/ParallelLibrary/)

	
	# New Output library
	file(GLOB_RECURSE ParallelLibrary
    "libraries/ParallelLibrary/*.cpp"
    "libraries/ParallelLibrary/*.h"
	)	
	
	# Base NC library
	add_library(ParallelLibrary STATIC ${ParallelLibrary})
	target_link_libraries(ParallelLibrary ${LIBS} ${NC_BASE} ${OUT_GUESS})