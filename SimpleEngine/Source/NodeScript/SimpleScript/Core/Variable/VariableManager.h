#pragma once
#include "../ScriptDefines.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace SCR
{

	struct Variable
	{
		DataTypeID dataTypeID = InvalidID<DataTypeID>();
		//MemoryPoolID runtimeMemoryID = InvalidID<MemoryPoolID>();
		//MemoryPoolID defaultValueMemoryID = InvalidID<MemoryPoolID>();
		void* runtimeDataPtr = nullptr;
		void* defaultValueDataPtr = nullptr;
		std::string name = "Var";
		bool isDestroyed = false;
	};

	class MemoryPool;
	class MemoryManager;

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

		std::unique_ptr<MemoryManager> myMemoryManager;
		//std::unique_ptr<MemoryPool> myMemoryPool;

	};
	
}