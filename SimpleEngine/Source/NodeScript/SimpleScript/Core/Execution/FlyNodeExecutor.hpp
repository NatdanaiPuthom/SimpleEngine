#pragma once
#include "../FlyDefines.hpp"
#include "FlyInternalExecutionContext.hpp"
#include "FlyCallStack.hpp"
#include <unordered_set>

namespace FLY_NAMESPACE
{

	class ClassInstance;

	class NodeExecutor final
	{
	public:

		NodeExecutor();
		~NodeExecutor();


		template<typename EventFunction, typename TargetType>
		void ExecuteEvent(EventFunction, ClassInstance& aClassInstance, TargetType* aTarget, const ExecutionContextBase& aContext, bool aExecuteAutoTickers = false);

		void ExecuteNode(const NodeExecutionData& aNodeExecutionData);

		void RegisterAutoTickNode(const NodeRef& aNodeRef);
		void UnregisterAutoTickNode(const NodeRef& aNodeRef);
		
		CallStack& GetCallStack()
		{
			return mCallStack;
		}


	private:

		void ExecuteEventInternal(EventID aEventID, ClassInstance& aClassInstance, void* aTarget, const ExecutionContextBase& aExecutionContext, bool aExecuteAutoTickers = false);

		bool IsSameTarget(const ClassInstance& aClassInstance, DataTypeID aDataTypeID) const;

	private:

		std::unordered_set<NodeExecutionData> mAutoTickNodes;

		InternalExecutionContext mExecutionContext;

		CallStack mCallStack;
	};

	template<typename EventFunction, typename TargetType>
	inline void NodeExecutor::ExecuteEvent(EventFunction aFunction, ClassInstance& aClassInstance, TargetType* const aTarget, const ExecutionContextBase& aContext, const bool aExecuteAutoTickers)
	{
		const EventID eventID = std::hash<EventFunction>()(aFunction);
		assert(IsSameTarget(aClassInstance, GetDataTypeID<TargetType>()));
		ExecuteEventInternal(eventID, aClassInstance, aTarget, aContext, aExecuteAutoTickers);
	}
}