#pragma once
#include "../ScriptDefines.hpp"
#include "../Memory/ScriptMemoryArena.hpp"
#include <unordered_map>

namespace FLY_NAMESPACE
{
	class NodeManager;

	class NodeManagerInstance final
	{
	public:

		NodeManagerInstance();
		~NodeManagerInstance();

		NodeManagerInstance(const NodeManagerInstance&) = delete;
		NodeManagerInstance& operator=(const NodeManagerInstance&) = delete;

		void Init(const NodeManager& aNodeManager);

		template<typename T>
		T& GetNodeState(NodeID aNodeID);

	private:

		std::unordered_map<NodeID, void*> mNodeStateMap;

	private:

		MemoryArena<1024> mMemoryArena;
	};

	template<typename T>
	inline T& NodeManagerInstance::GetNodeState(NodeID aNodeID)
	{
		return *reinterpret_cast<T*>(mNodeStateMap.at(aNodeID));
	}
}