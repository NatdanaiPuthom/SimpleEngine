#pragma once
#include "../ScriptDefines.h"
#include "NodeRef.h"
#include <queue>

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
		NodeRef nodeRef;
		eNodeTriggerReason triggerReason = eNodeTriggerReason::Flow;
	};

	inline bool operator==(const NodeExecutionData& a, const NodeExecutionData& b)
	{
		return a.nodeRef == b.nodeRef;
	}

	inline bool operator<(const NodeExecutionData& a, const NodeExecutionData& b)
	{
		return a.nodeRef < b.nodeRef;
	}

	class NodeExecutor;

	class ExecutionQueue
	{
	public:

		ExecutionQueue();
		~ExecutionQueue();

		void Push(const NodeExecutionData& aNode);
		void Execute();

	private:


		std::queue<NodeExecutionData> myExecutionQueue;

	};

	class Script;
	class InternalModifier;
	struct ExecutionContextBase;
	class ScriptInstance;

	struct InternalExecutionContext final
	{
		friend class NodeExecutor;

		Script* script;
		const ExecutionContextBase* executionContext = nullptr;
		NodeExecutionData nodeData;
		ExecutionQueue* executionQueue = nullptr;
		void* owner = nullptr;
		ScriptInstance* scriptInstance;

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
		return static_cast<size_t>(aValue.nodeRef.nodeID);
	}
};