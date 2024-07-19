#pragma once
#include "../ScriptDefines.h"
#include "../Memory/ScriptMemoryArena.h"
#include <unordered_map>

namespace SCR
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

		std::unordered_map<NodeID, void*> myNodeStateMap;

	private:

		MemoryArena<1024> myMemoryArena;
	};

	template<typename T>
	inline T& NodeManagerInstance::GetNodeState(NodeID aNodeID)
	{
		return *reinterpret_cast<T*>(myNodeStateMap.at(aNodeID));
	}
}