#pragma once
#include <string>
#include "BaseFileNC.h"
#include "InternalWriterNC.h"

namespace nc_internal
{
	class InternalReader: virtual public BaseFileNC
	{
	protected:
		InternalReader(string filename);
		InternalReader();
		~InternalReader();
	public:
		
		void Inquery();
		void PrintInfos();
		Group& GetMainGroup();
		vector<Group> GetAllGroups();
		Group& GetGroup(string name);
		Variable& GetVariable(string grpname, string varname);

	protected:
		void InqGroups();

		void InqBaseInfo(Group& group);
		void InqGroupAttributes(Group& group);
		void InqDimensions(Group& group);
		void InqVariables(Group& group, size_t n_vars);

		void InqAttributes(Variable& var);
		void InqDimensionIndicies(Variable& var, size_t ndims);
		void InqChunckSizes(Variable& var);

		string GetAttributeValue(const Variable& var, Attribute& attr, const vector<char>& internalName);
		string CleanString(string inputStr) const;
	};
}

