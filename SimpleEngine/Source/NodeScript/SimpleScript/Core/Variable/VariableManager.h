#pragma once
#include "../ScriptDefines.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace SCR
{

	struct Variable
	{
		DataTypeID dataTypeID = InvalidID<DataTypeID>();
		MemoryPoolID runtimeMemoryID = InvalidID<MemoryPoolID>();
		MemoryPoolID defaultValueMemoryID = InvalidID<MemoryPoolID>();
		std::string name = "Var";
		bool isDestroyed = false;
	};

	class Script;

	class VariableManager
	{
		friend class ScriptProxy;
	public:

		VariableManager();
		~VariableManager();


		VarID GetVariableIDByNodeID(const NodeID aNodeID) const;
		std::vector<NodeID> GetNodeIDsByVarID(const VarID aVarID) const;

	private:

		std::vector<Variable> myVariables;

		std::unordered_map<NodeID, VarID> myNodeIDToVarID;


	};
	
}