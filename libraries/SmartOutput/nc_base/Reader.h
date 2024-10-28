#pragma once
#include "InternalReader.h"

using namespace nc_internal;

namespace nc_base
{
	class Reader : public InternalReader
	{
	public:
		/**
		* \brief Creates a NetCDF reader, which can only open files and is not able to modify the content.
		* \param filename Name of the file to be openend
		*/
		Reader(string filename);

		// Reader and WriterNC use the same BaseFile members, to make them the work together, the reader properties have to be dynamically
		// allocated, as they cannot be created from outside the class
		// Todo Make pointer unqiue or shared
		// Todo Move this to InternalReader
		~Reader();
	};

}

