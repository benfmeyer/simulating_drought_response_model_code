#include "WriterNC.h"


WriterNC::WriterNC(const string filename, const Mode openmode, const DataFormat data_format): InternalReader(filename), InternalWriterNC(filename), BaseFileNC(filename)
{
	// Call the BaseFile-Initializer. Resharper thinks that the Basefile qualifier is redundant
	// however this is not the case, at least not in C++14!
	BaseFileNC::Init(openmode, data_format);
}

WriterNC::WriterNC(): InternalReader(), InternalWriterNC(), BaseFileNC()
{
}

WriterNC::~WriterNC()
{

}

void WriterNC::Close()
{
	BaseFileNC::Close();
}

