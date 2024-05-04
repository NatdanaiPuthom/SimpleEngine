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
		NodeID currentNodeID = InvalidID<NodeID>();
		eNodeTriggerReason triggerReason = eNodeTriggerReason::Flow;
	};

	inline bool operator==(const NodeExecutionData& aValue1, const NodeExecutionData& aValue2)
	{
		return aValue1.currentNodeID == aValue2.currentNodeID;
	}

	inline bool operator<(const NodeExecutionData& aValue1, const NodeExecutionData& aValue2)
	{
		return aValue1.currentNodeID < aValue2.currentNodeID;
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

		const NodeExecutionData& GetNodeData() const
		{
			return nodeData;
		}

	private:
		NodeExecutionData nodeData;
	};
}

template<>
struct std::hash<SCR::NodeExecutionData>
{
	std::size_t operator()(const SCR::NodeExecutionData& aValue) const
	{
		return static_cast<size_t>(aValue.currentNodeID);
	}
};