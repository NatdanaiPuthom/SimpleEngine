#pragma once
#include "../FlyDefines.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "../Node/FlyNodeRef.hpp"
#include "FlyVariable.hpp"

namespace FLY_NAMESPACE
{

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

		std::vector<Variable> mVariables;

		std::unordered_map<NodeRef, VarID, NodeRefHasher> mNodeRefToVarID;

		std::unique_ptr<MemoryArena<NodeBufferCapacity>> mMemoryArena;

	};
	
}