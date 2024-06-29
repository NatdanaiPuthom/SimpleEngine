#include "ExecutionNodes.h"
#include "../Node/NodeTypeRegistry.h"

namespace SCR
{

	std::tuple<Flow, float> Tick(NodeExecutionContext<ExecutionContextBase> aContext)
	{
		return { Flow(true), aContext.context.deltaTime };
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
		return { aCondition, !aCondition };
	}

	struct FlipFlopNodeData
	{
		bool myState = true;
	};

	static std::tuple<Flow, Flow> FlipFlop(NodeState<FlipFlopNodeData> aData, Flow)
	{
		aData.value.myState = !aData.value.myState;
		return { !aData.value.myState, aData.value.myState };
	}

	struct DelayNodeData
	{
		float time = 0.f;
	};

	static Flow Delay(const InternalExecutionContext* aContext, NodeState<DelayNodeData> aState, Flow, float aDuration, bool aResetOnFlow)
	{
		if (aContext->GetNodeData().triggerReason == eNodeTriggerReason::Flow)
		{
			if (aResetOnFlow)
			{
				aState.value.time = 0.f;
			}
			ScriptProxy::GetNodeExecutor().RegisterAutoTickNode(aContext->GetNodeData().nodeRef);
		}
		aState.value.time += aContext->executionContext->deltaTime;
		if (aState.value.time > aDuration)
		{
			aState.value.time = 0.f;
			ScriptProxy::GetNodeExecutor().UnregisterAutoTickNode(aContext->GetNodeData().nodeRef);
			return true;
		}
		return false;
	}

	REGISTER_FUNCTION(BeginPlay, "Events", Event);
	REGISTER_FUNCTION(Tick, "Events", Event);
	REGISTER_FUNCTION(EndPlay, "Events", Event);

	REGISTER_FUNCTION(Branch, "Execution");
	REGISTER_FUNCTION(FlipFlop, "Execution");
	REGISTER_FUNCTION(Delay, "Execution");

}
