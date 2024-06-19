#include "ExecutionNodes.h"
#include "../Node/NodeTypeRegistry.h"
#include "../Contexts/ExecutionContextBase.h"
#include "DataTypeRegistry.h"
#include "ScriptInstance.h"

namespace SCR
{

	static std::tuple<Flow, float> TickNode(NodeExecutionContext<ExecutionContextBase> aContext)
	{
		return { Flow(true), aContext.context.deltaTime };
	}

	static Flow BeginPlayNode()
	{
		return { true };
	}


	static Flow EndPlayNode()
	{
		return { true };
	}

	static std::tuple<Flow, Flow> BranchNode(Flow, bool aCondition)
	{
		return { aCondition, !aCondition };
	}

	struct FlipFlopNodeData
	{
		bool myState = true;
	};

	static std::tuple<Flow, Flow> FlipFlopNode(NodeState<FlipFlopNodeData> aData, Flow)
	{
		aData.value.myState = !aData.value.myState;
		return { !aData.value.myState, aData.value.myState };
	}

	struct DelayNodeData
	{
		float time = 0.f;
	};

	static Flow DelayNode(const InternalExecutionContext* aContext, NodeState<DelayNodeData> aState, Flow, float aDuration, bool aResetOnFlow)
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

	static std::tuple<int, bool> FloatNode(float& aValue, std::string& aName)
	{
		aValue += 1.f;
		std::cout << aName << std::endl;
		aName += "E";
		return { 4, true };
	}

	REGISTER_FUNCTION(BeginPlayNode, Event);

	class Emil
	{
	public:


		int GetI() const
		{
			return i;
		}
	private:

		int i = 5;
	};

	void RegisterExecutionNodes()
	{

		DataTypeRegistry::RegisterNonSerializableType<Emil>("Emil");

		NodeTypeRegistry::RegisterMemberNodeType(&Emil::GetI, "Test/Get I");

		NodeTypeRegistry::RegisterFlowNodeType(FloatNode, "Test/FLoat");

		NodeTypeRegistry::RegisterNodeType<eNodeEventType::Tick>(TickNode, "Events/Event Tick", NodeTypeDesc{ { }, { "Flow", "Delta Time" } });
		NodeTypeRegistry::RegisterNodeType<eNodeEventType::BeginPlay>(BeginPlayNode, "Events/Event Begin Play", NodeTypeDesc{ { }, { "Flow" } });
		NodeTypeRegistry::RegisterNodeType<eNodeEventType::EndPlay>(EndPlayNode, "Events/Event End Play", NodeTypeDesc{ { }, { "Flow" } });

		NodeTypeRegistry::RegisterNodeType(BranchNode, "Execution/Branch", NodeTypeDesc{ { "Flow", "Condition" }, { "True", "False" } });
		NodeTypeRegistry::RegisterNodeType(FlipFlopNode, "Execution/FlipFlop", NodeTypeDesc{ { "Flow" }, { "Flip", "Flop" } });
		NodeTypeRegistry::RegisterNodeType(DelayNode, "Execution/Delay", NodeTypeDesc{ { "Flow", "Duration", "Reset On Flow" }, {"Flow"} });
	}

}
