#pragma once
#include <string>
#include "Dimension.h"
#include "Group.h"
#include "OpenMode.h"
//#include <mpi.h>

using std::string;
using namespace nc_base;

class BaseFile
{

protected:
	BaseFile(string filename);
	//Empty basefile
	BaseFile();
	//virtual void Init(Mode mode) = 0;

public:
	virtual void Close() = 0;
	virtual ~BaseFile();

protected:
	string filename_;
	int main_ncid_;
	/// Id only required for group definitions
	int parrent_ncid;
	Group* main_group_;
	vector<Group*> groups_;
	vector<Dimension*> dimensions_;
	nc_internal::DataFormat file_format_;
};

