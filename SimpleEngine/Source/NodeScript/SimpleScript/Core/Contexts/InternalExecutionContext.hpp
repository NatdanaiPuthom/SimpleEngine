#pragma once
#include "../ScriptDefines.hpp"
#include "NodeRef.hpp"
#include <queue>

namespace FLY_NAMESPACE
{


	enum class eNodeTriggerReason
	{
		Flow,
		Event,
		Read
	};

	struct NodeExecutionData final
	{
		NodeRef mNodeRef;
		eNodeTriggerReason mTriggerReason = eNodeTriggerReason::Flow;
	};

	inline bool operator==(const NodeExecutionData& a, const NodeExecutionData& b)
	{
		return a.mNodeRef == b.mNodeRef;
	}

	inline bool operator<(const NodeExecutionData& a, const NodeExecutionData& b)
	{
		return a.mNodeRef < b.mNodeRef;
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


		std::queue<NodeExecutionData> mExecutionQueue;

	};

	class Class;
	class InternalModifier;
	struct ExecutionContextBase;
	class ClassInstance;
	class NodeGraphInstance;

	struct InternalExecutionContext final
	{
		Class* mClass = nullptr;
		const ExecutionContextBase* mExecutionContext = nullptr;
		NodeExecutionData mNodeData;
		ExecutionQueue* mExecutionQueue = nullptr;
		void* mOwner = nullptr;
		ClassInstance* mClassInstance = nullptr;
		NodeGraphInstance* mNodeGraphInstance = nullptr;
		NodeExecutor* mNodeExecutor = nullptr;

	private:
	};
}

template<>
struct std::hash<FLY_NAMESPACE::NodeExecutionData>
{
	std::size_t operator()(const FLY_NAMESPACE::NodeExecutionData& aValue) const
	{
		return static_cast<size_t>(aValue.mNodeRef.mNodeID);
	}
};