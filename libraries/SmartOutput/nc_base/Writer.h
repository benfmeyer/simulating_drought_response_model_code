#pragma once
#include <vector>
#include "Dimension.h"

namespace nc_base {
	enum class Mode;
	struct Group;
}

class Writer 
{
public:
	Writer();
	virtual ~Writer();

	virtual void AddGroup(nc_base::Group& group) = 0;
	virtual void Reopen(string filename, nc_base::Mode mode) = 0;
	virtual void SetDimensions(std::vector<nc_base::Dimension>& dimensions) = 0;
	virtual void SetMainGroup(nc_base::Group& main_group) = 0;
	virtual void SetGroups(std::vector<nc_base::Group>& groups) = 0;
	virtual void EndDef() = 0;
	virtual void Close() = 0;
};

