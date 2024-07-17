#pragma once
#include "../ScriptDefines.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "../Node/NodeRef.h"

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

	class VariableManager final
	{
		friend class ScriptProxy;
	public:

		VariableManager();
		~VariableManager();

		VariableManager(const VariableManager&) = delete;
		VariableManager(VariableManager&&) = default;
		VariableManager& operator=(const VariableManager&) = delete;
		VariableManager& operator=(VariableManager&&) = default;


		VarID GetVariableIDByNodeRef(const NodeRef& aNodeRef) const;
		std::vector<NodeRef> GetNodeRefsByVarID(const VarID aVarID) const;

	public:

		std::vector<Variable> myVariables;

		std::unordered_map<NodeRef, VarID, NodeRefHasher> myNodeRefToVarID;

		std::unique_ptr<MemoryArena<NodeBufferCapacity>> myMemoryArena;

	};
	
}