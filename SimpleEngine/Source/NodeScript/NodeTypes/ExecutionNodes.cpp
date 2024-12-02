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

	std::tuple<Flow, Flow> Branch(Flow, bool aCondition)
	{
		return { Flow(aCondition), Flow(!aCondition) };
	}


	std::tuple<Flow, int, Flow> ForLoop(InternalExecutionContextPtr aContext, Flow, int aStartIndex, int aEndIndex, int aIncrement)
	{
		ExecutionQueue* previousQueue = aContext->mExecutionQueue;
		NodeExecutionData nodeExecutionData = aContext->mNodeData;
		const Node& node = nodeExecutionData.mNodeRef.GetNodeGraph().mNodes[nodeExecutionData.mNodeRef.GetNodeID()];

		for (int i = aStartIndex; i < aEndIndex; i += aIncrement)
		{
			ExecutionQueue executionQueue;

			aContext->mExecutionQueue = &executionQueue;

			SetOutputValues(std::tuple{ Flow(true), i }, node.mOutputPins, *aContext);
			
			executionQueue.Execute();

			aContext->mNodeData = nodeExecutionData;

		}

		aContext->mExecutionQueue = previousQueue;
		aContext->mNodeData = nodeExecutionData;

		return { Flow(false), 0, Flow(true) };
	}


	struct FlipFlopNodeData
	{
		bool myState = true;
	};

	std::tuple<Flow, Flow> FlipFlop(NodeState<FlipFlopNodeData> aData, Flow)
	{
		aData.mValue.myState = !aData.mValue.myState;
		return { Flow(!aData.mValue.myState), Flow(aData.mValue.myState) };
	}

	struct DelayNodeData
	{
		float time = 0.f;
	};

	Flow Delay(InternalExecutionContextPtr aContext, NodeState<DelayNodeData> aState, Flow, float aDuration, bool aResetOnFlow)
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

	FLY_FUNCTION(ForLoop, Directory{ "Execution" }, InputNames{ "Flow", "Start Index", "End Index", "Increment" }, OutputNames{ "Flow", "Index", "Complete" }, DefaultValues{ Flow(true), 0, 0, 1 }, Pure{});
	FLY_FUNCTION(Branch, Directory{ "Execution" }, InputNames{ "Flow", "Condition" }, OutputNames{ "True", "False" }, Pure{});
	FLY_FUNCTION(FlipFlop, Directory{ "Execution" }, OutputNames{ "Flip", "Flop" }, Pure{});
	FLY_FUNCTION(Delay, Directory{ "Execution" }, InputNames{ "Flow", "Duration", "Reset On Flow" }, DefaultValues{ Flow(false), 1.f, false }, AutoTick{}, Pure{});
}
