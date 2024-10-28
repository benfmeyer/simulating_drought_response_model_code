#include "BaseFile.h"


BaseFile::BaseFile(string filename) :main_ncid_(-1), parrent_ncid(-1)
{
	filename_ = filename;
}


BaseFile::BaseFile() : main_ncid_(-1), parrent_ncid(-1), main_group_(nullptr)
{
}


BaseFile::~BaseFile()
{
}
