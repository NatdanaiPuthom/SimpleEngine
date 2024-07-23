#include "ExecutionNodes.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	std::tuple<Flow, float> Tick(NodeExecutionContext<ExecutionContextBase> aContext)
	{
		return { Flow(true), aContext.mContext.mDeltaTime };
	}

	Flow BeginPlay()
	{
		return Flow(true);
	}

	Flow EndPlay()
	{
		return Flow(true);
	}

	static std::tuple<Flow, Flow> Branch(Flow, bool aCondition)
	{
		return { Flow(aCondition), Flow(!aCondition) };
	}

	struct FlipFlopNodeData
	{
		bool myState = true;
	};

	static std::tuple<Flow, Flow> FlipFlop(NodeState<FlipFlopNodeData> aData, Flow)
	{
		aData.mValue.myState = !aData.mValue.myState;
		return { Flow(!aData.mValue.myState), Flow(aData.mValue.myState) };
	}

	struct DelayNodeData
	{
		float time = 0.f;
	};

	static Flow Delay(const InternalExecutionContext* aContext, NodeState<DelayNodeData> aState, Flow, float aDuration, bool aResetOnFlow)
	{
		if (aContext->mNodeData.mTriggerReason == eNodeTriggerReason::Flow)
		{
			if (aResetOnFlow)
			{
				aState.mValue.time = 0.f;
			}
			aContext->mNodeExecutor->RegisterAutoTickNode(aContext->mNodeData.mNodeRef);
		}
		aState.mValue.time += aContext->mExecutionContext->mDeltaTime;
		if (aState.mValue.time > aDuration)
		{
			aState.mValue.time = 0.f;
			aContext->mNodeExecutor->UnregisterAutoTickNode(aContext->mNodeData.mNodeRef);
			return Flow(true);
		}
		return Flow(false);
	}

	REGISTER_FUNCTION(BeginPlay, "Events", Event);
	REGISTER_FUNCTION(Tick, "Events", Event);
	REGISTER_FUNCTION(EndPlay, "Events", Event);

	REGISTER_FUNCTION(Branch, "Execution");
	REGISTER_FUNCTION(FlipFlop, "Execution");
	REGISTER_FUNCTION(Delay, "Execution");

}
