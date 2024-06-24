#pragma once
#include "../ScriptDefines.h"
#include "../Memory/ScriptMemoryManager.h"
#include <unordered_map>

namespace SCR
{
	class NodeManager;
	class MemoryManager;

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

		std::unordered_map<NodeID, void*> myNodeStateMap;

	private:

		MemoryManager myMemoryManager;
	};

	template<typename T>
	inline T& NodeManagerInstance::GetNodeState(NodeID aNodeID)
	{
		return *reinterpret_cast<T*>(myNodeStateMap.at(aNodeID));
	}
}