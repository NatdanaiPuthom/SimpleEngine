#include "ExecutionNodes.hpp"

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
			aContext->mClassInstance->mClass->mEventGraph.BindNodeToEvent(aContext->mNodeData.mNodeRef.GetNodeID());
		}
		aState.mValue.time += aContext->mExecutionContext->mDeltaTime;
		if (aState.mValue.time > aDuration)
		{
			aState.mValue.time = 0.f;
			aContext->mClassInstance->mClass->mEventGraph.UnbindNodeFromEvent(aContext->mNodeData.mNodeRef.GetNodeID());
			return Flow(true);
		}
		return Flow(false);
	}

	FLY_FUNCTION(BeginPlay, Directory{ "Events" }, Event{}, Pure{});
	FLY_FUNCTION(Tick, Directory{ "Events" }, Event{}, OutputNames{ "Flow", "Delta Time" }, Pure{});
	FLY_FUNCTION(EndPlay, Directory{ "Events" }, Event{}, Pure{});

	FLY_FUNCTION(Branch, Directory{ "Execution" }, InputNames{ "Flow", "Condition" }, OutputNames{ "True", "False" }, DefaultValues{ Flow(false), true }, Pure{});
	FLY_FUNCTION(FlipFlop, Directory{ "Execution" }, OutputNames{ "Flip", "Flop" }, Pure{});
	FLY_FUNCTION(Delay, Directory{ "Execution" }, InputNames{ "Flow", "Duration", "Reset On Flow" }, DefaultValues{ Flow(false), 1.f, false }, AutoTick{}, Pure{});

}
