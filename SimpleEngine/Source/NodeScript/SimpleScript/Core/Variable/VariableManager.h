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
		void* defaultValueDataPtr = nullptr;
		std::string name = "Var";
		bool isDestroyed = false;
	};

	template<size_t> class MemoryArena;

	class VariableManager
	{
		friend class ScriptProxy;
	public:

		VariableManager();
		~VariableManager();

		VariableManager(const VariableManager&) = delete;
		VariableManager(VariableManager&&) = default;
		VariableManager& operator=(const VariableManager&) = delete;
		VariableManager& operator=(VariableManager&&) = default;


		VarID GetVariableIDByNodeID(const NodeID aNodeID) const;
		std::vector<NodeID> GetNodeIDsByVarID(const VarID aVarID) const;

	public:

		std::vector<Variable> myVariables;

		std::unordered_map<NodeID, VarID> myNodeIDToVarID;

		std::unique_ptr<MemoryArena<NodeBufferCapacity>> myMemoryArena;

	};
	
}