	#add_subdirectory(out_guess)
	#include_directories(out_guess)	

	#add_subdirectory(nc_guess)
	#include_directories(nc_guess)

	#add_subdirectory(nc_base)
	include_directories(libraries/SmartOutput/nc_base/)
	include_directories(libraries/SmartOutput/nc_guess/)
	include_directories(libraries/SmartOutput/out_guess/)
	
	
	# New Output library
	file(GLOB_RECURSE nc_base
    "libraries/SmartOutput/nc_base/*.cpp"
    "libraries/SmartOutput/nc_base/*.h"
	)
	
	
	# Base NC library
	add_library(NC_BASE STATIC ${nc_base})
	target_link_libraries(NC_BASE ${LIBS})
	
	

	
	# New Output library
	file(GLOB_RECURSE out_guess
    "libraries/SmartOutput/out_guess/*.cpp"
    "libraries/SmartOutput/out_guess/*.h"
	)
	
	add_library(OUT_GUESS STATIC ${out_guess})
	target_link_libraries(OUT_GUESS ${LIBS})

	# Guess NC library
	file(GLOB_RECURSE nc_guess_internal_out
    "libraries/SmartOutput/nc_guess/internal_out/*.cpp"
    "libraries/SmartOutput/nc_guess/internal_out/*.h"
	)
	
	file(GLOB_RECURSE nc_guess_internal_in
    "libraries/SmartOutput/nc_guess/internal_in/*.cpp"
    "libraries/SmartOutput/nc_guess/internal_in/*.h"
	)

	file(GLOB_RECURSE nc_guess_lib
    "libraries/SmartOutput/nc_guess/lib/*.cpp"
    "libraries/SmartOutput/nc_guess/lib/*.h"
	)
	
	add_library(NC_GUESS STATIC ${nc_guess_internal_out} ${nc_guess_internal_in} ${nc_guess_lib})
	target_link_libraries(NC_GUESS ${LIBS} ${NC_BASE} ${OUT_GUESS})
	
	source_group("internal_in" FILES ${nc_guess_internal_in})
	source_group("internal_out" FILES ${nc_guess_internal_out}) 	
	source_group("lib" FILES ${nc_guess_lib}) 
