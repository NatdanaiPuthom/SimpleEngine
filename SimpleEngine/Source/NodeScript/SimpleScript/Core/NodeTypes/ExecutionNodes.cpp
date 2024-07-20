#include "ExecutionNodes.hpp"
#include "../Node/NodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	std::tuple<Flow, float> Tick(NodeExecutionContext<ExecutionContextBase> aContext)
	{
		return { Flow(true), aContext.context.mDeltaTime };
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
		aData.value.myState = !aData.value.myState;
		return { Flow(!aData.value.myState), Flow(aData.value.myState) };
	}

	struct DelayNodeData
	{
		float time = 0.f;
	};

	static Flow Delay(const InternalExecutionContext* aContext, NodeState<DelayNodeData> aState, Flow, float aDuration, bool aResetOnFlow)
	{
		if (aContext->GetNodeData().mTriggerReason == eNodeTriggerReason::Flow)
		{
			if (aResetOnFlow)
			{
				aState.value.time = 0.f;
			}
			ScriptProxy::GetNodeExecutor().RegisterAutoTickNode(aContext->GetNodeData().mNodeRef);
		}
		aState.value.time += aContext->mExecutionContext->mDeltaTime;
		if (aState.value.time > aDuration)
		{
			aState.value.time = 0.f;
			ScriptProxy::GetNodeExecutor().UnregisterAutoTickNode(aContext->GetNodeData().mNodeRef);
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
