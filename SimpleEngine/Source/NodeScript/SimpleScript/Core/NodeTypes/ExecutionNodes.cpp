#include "ExecutionNodes.h"
#include "../Node/NodeTypeRegistry.h"
#include "../Contexts/ExecutionContextBase.h"

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

	static Flow DelayNodeTest(const InternalExecutionContext* aContext, NodeState<DelayNodeData> aState, Flow, float aDuration, bool aResetOnFlow)
	{
		if (aContext->GetNodeData().triggerReason == eNodeTriggerReason::Flow)
		{
			if (aResetOnFlow)
			{
				aState.value.time = 0.f;
			}
			ScriptProxy::GetNodeExecutor(aContext->script).RegisterAutoTickNode(aContext->GetNodeData().nodeID);
		}
		aState.value.time += aContext->executionContext->deltaTime;
		if (aState.value.time > aDuration)
		{
			aState.value.time = 0.f;
			ScriptProxy::GetNodeExecutor(aContext->script).UnregisterAutoTickNode(aContext->GetNodeData().nodeID);
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

	enum class eEventType1
	{
		eEvent,
		eEvent2
	};

	enum class eEventType2
	{

		eEvent,
		eEvent2
	};

	void RegisterExecutionNodes()
	{

		size_t t1 = typeid(eEventType1).hash_code();
		size_t t2 = typeid(eEventType2).hash_code();
		t1;
		t2;
		NodeTypeRegistry::RegisterFlowNodeType(FloatNode, "Test/FLoat");

		NodeTypeRegistry::RegisterNodeType<eNodeExecutionTrait::Tick>(TickNode, "Execution/Event Tick", NodeTypeDesc{ { }, { "Flow", "Delta Time" } });
		NodeTypeRegistry::RegisterNodeType<eNodeExecutionTrait::BeginPlay>(BeginPlayNode, "Execution/Event Begin Play", NodeTypeDesc{ { }, { "Flow" } });
		NodeTypeRegistry::RegisterNodeType<eNodeExecutionTrait::EndPlay>(EndPlayNode, "Execution/Event End Play", NodeTypeDesc{ { }, { "Flow" } });
		NodeTypeRegistry::RegisterNodeType(BranchNode, "Execution/Branch", NodeTypeDesc{ { "Flow", "Condition" }, { "True", "False" } });
		NodeTypeRegistry::RegisterNodeType(FlipFlopNode, "Execution/FlipFlop", NodeTypeDesc{ { "Flow" }, { "Flip", "Flop" } });
		NodeTypeRegistry::RegisterNodeType(DelayNodeTest, "Execution/Delay", NodeTypeDesc{ { "Flow", "Duration", "Reset On Flow" }, {"Flow"} });
	}

}
