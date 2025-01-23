#pragma once
#include "../FlyDefines.hpp"
#include "../Node/FlyNodeRef.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include <queue>

namespace FLY_NAMESPACE
{


	enum class eNodeTriggerReason : uint8_t
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

	class Class;
	class InternalModifier;
	struct ExecutionContextBase;
	class ClassInstance;
	class NodeGraphInstance;
	class NodeExecutor;
	class NodeExecutionQueue;
	class NodeTypeManager;
	class PinTypeManager;
	class DataTypeManager;
	class TraitManager;
	class MemoryPool;

	struct InternalExecutionContext final
	{
		Class* mClass = nullptr;
		const ExecutionContextBase* mExecutionContext = nullptr;
		NodeExecutionData mNodeData;
		NodeGraphVariantHandle mNodeGraphVariantHandle;
		NodeExecutionQueue* mNodeExecutionQueue = nullptr;
		void* mTarget = nullptr;
		ClassInstance* mClassInstance = nullptr;
		NodeGraphInstance* mNodeGraphInstance = nullptr;
		NodeExecutor* mNodeExecutor = nullptr;
		NodeTypeManager* mNodeTypeManager = nullptr;
		PinTypeManager* mPinTypeManager = nullptr;
		DataTypeManager* mDataTypeManager = nullptr;
		TraitManager* mTraitManager = nullptr;
		MemoryPool* mFoundationMemoryPool = nullptr;
	};
}

template<>
struct std::hash<FLY_NAMESPACE::NodeExecutionData>
{
	std::size_t operator()(const FLY_NAMESPACE::NodeExecutionData& aValue) const
	{
		return static_cast<size_t>(aValue.mNodeRef.GetNodeID());
	}
};