#pragma once
#include "../ScriptDefines.h"

namespace SCR
{


	enum class eNodeTriggerReason
	{
		Flow,
		Event,
		Read
	};

	struct NodeExecutionData final
	{
		NodeID nodeID = InvalidID<NodeID>();
		eNodeTriggerReason triggerReason = eNodeTriggerReason::Flow;
	};

	inline bool operator==(const NodeExecutionData& aValue1, const NodeExecutionData& aValue2)
	{
		return aValue1.nodeID == aValue2.nodeID;
	}

	inline bool operator<(const NodeExecutionData& aValue1, const NodeExecutionData& aValue2)
	{
		return aValue1.nodeID < aValue2.nodeID;
	}


	class Script;
	class ScriptInternalModifier;
	struct ExecutionContextBase;

	struct InternalExecutionContext final
	{
		friend class NodeExecutor;

		InternalExecutionContext(Script& aScript, ScriptInternalModifier& aModifier)
			: script(aScript)
			, modifier(aModifier)
		{

		}

		Script& script;
		ScriptInternalModifier& modifier;
		const ExecutionContextBase* executionContext = nullptr;
		NodeExecutionData nodeData;

		const NodeExecutionData& GetNodeData() const
		{
			return nodeData;
		}

		const Node& GetCurrentNode() const;

	private:
	};
}

template<>
struct std::hash<SCR::NodeExecutionData>
{
	std::size_t operator()(const SCR::NodeExecutionData& aValue) const
	{
		return static_cast<size_t>(aValue.nodeID);
	}
};