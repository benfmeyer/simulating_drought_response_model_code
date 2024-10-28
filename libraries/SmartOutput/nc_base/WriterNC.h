#pragma once
#include "InternalReader.h"
#include "InternalWriterNC.h"
namespace nc_base
{
	using namespace nc_internal;

	class WriterNC : public virtual InternalWriterNC, public virtual InternalReader
	{
	public:
		/**
		 * \brief NetCDF WriterNC class, which can open both read and writer.
		 * \param filename Name of file to open or create
		 * \param openmode various opening modes, parallel can only be selected if PNetCDF is available, Create overrides 
		 * exisiting files
		 */
		WriterNC(string filename, Mode openmode, DataFormat data_format);

		//Empty writer
		WriterNC();

		// Memory does not need to be taken care of, as the writing variables are defined outside the class
		// and the class itself only uses references and pointers to the variables.
		~WriterNC();
		void Close() override;
	};
}

