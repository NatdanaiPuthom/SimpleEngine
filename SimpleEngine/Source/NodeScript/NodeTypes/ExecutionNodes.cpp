#include "ExecutionNodes.hpp"
#include "imgui/imgui.h"

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

	std::tuple<Flow, Flow, Flow, Flow> Sequence(Flow)
	{
		return { Flow(true), Flow(true), Flow(true), Flow(true) };
	}

	enum class eComparatorType
	{
		Equal,
		NotEqual,
		Greater,
		GreaterEqual,
		Less,
		LessEqual
	};

	template<>
	struct EnumCount<eComparatorType> : EnumCountConstant<6> {};

	template<typename T>
	struct IsLayeredEnum : std::false_type {};

	std::string EnumToString(const eComparatorType aComparatorType)
	{
		switch (aComparatorType)
		{
		case eComparatorType::Equal:
			return "Equal";
			break;
		case eComparatorType::NotEqual:
			return "Not Equal";
			break;
		case eComparatorType::Greater:
			return "Greater";
			break;
		case eComparatorType::GreaterEqual:
			return "Greater Equal";
			break;
		case eComparatorType::Less:
			return "Less";
			break;
		case eComparatorType::LessEqual:
			return "Less Equal";
			break;
		default:
			assert(false);
			return "None";
			break;
		}
	}

	template<Enumerable T>
	ViewAndEditResult ViewAndEdit(T& aEnumValue)
	{
		ViewAndEditResult viewAndEditResult;
		if constexpr (IsLayeredEnum<T>::value)
		{
			for (size_t i = 0; i < EnumCount<T>::value; i++)
			{
				const T current = static_cast<T>(static_cast<size_t>(1) << i);
				const bool isActive = (aEnumValue & current) != static_cast<T>(0);
				if (ImGui::RadioButton(EnumToString(current).c_str(), isActive))
				{
					if (isActive)
					{
						aEnumValue ^= current;
					}
					else
					{
						aEnumValue |= current;
					}
					viewAndEditResult.mIsItemActive |= ImGui::IsItemActive();
				}
			}
		}
		else
		{
			if (ImGui::BeginCombo("##", EnumToString(aEnumValue).c_str()))
			{
				for (size_t i = 0; i < EnumCount<T>::value; i++)
				{
					const T current = static_cast<T>(i);
					if (ImGui::Selectable(EnumToString(current).c_str(), aEnumValue == current))
					{
						aEnumValue = current;
					}
				}

				viewAndEditResult.mIsItemActive = true;

				ImGui::EndCombo();
			}
		}

		return viewAndEditResult;
	}

	template<Enumerable T>
	void Save(const T& aEnumValue, nlohmann::json& aJson)
	{
		aJson["Value"] = aEnumValue;
	}

	template<Enumerable T>
	void Load(T& aEnumValue, const nlohmann::json& aJson)
	{
		aEnumValue = aJson["Value"];
	}

	enum class eColLayer : size_t
	{
		None = 0,
		Player = 1 << 0,
		Enemy = 1 << 1,
		Wall = 1 << 2,
		Item = 1 << 3
	};

	std::string EnumToString(const eColLayer aColLayer)
	{
		switch (aColLayer)
		{
		case Fly::eColLayer::None:
			return "None";
			break;
		case Fly::eColLayer::Player:
			return "Player";
			break;
		case Fly::eColLayer::Enemy:
			return "Enemy";
			break;
		case Fly::eColLayer::Wall:
			return "Wall";
			break;
		case Fly::eColLayer::Item:
			return "Item";
			break;
		default:
			assert(false);
			return "Null";
			break;
		}
	}

	template<>
	struct EnumCount<eColLayer> : EnumCountConstant<4> {};

	template<>
	struct IsLayeredEnum<eColLayer> : std::bool_constant<true> {};

	void TestColLayer(eColLayer)
	{

	}

	FLY_VALUETYPE(eComparatorType);
	FLY_VALUETYPE(eColLayer);

	FLY_FUNCTION(TestColLayer);

	template<template<typename> typename Comparator, typename T>
	bool Compare(const T& a, const T& b)
	{
		return Comparator<T>{}(a, b);
	}

	bool Compare(const auto& a, const auto& b, const eComparatorType aComparatorType)
	{
		switch (aComparatorType)
		{
		case eComparatorType::Equal:
			return Compare<std::equal_to>(a, b);
			break;
		case eComparatorType::NotEqual:
			return Compare<std::not_equal_to>(a, b);
			break;
		case eComparatorType::Greater:
			return Compare<std::greater>(a, b);
			break;
		case eComparatorType::GreaterEqual:
			return Compare<std::greater_equal>(a, b);
			break;
		case eComparatorType::Less:
			return Compare<std::less>(a, b);
			break;
		case eComparatorType::LessEqual:
			return Compare<std::less_equal>(a, b);
			break;
		default:
			assert(false);
			break;
		}
		return false;
	}

	std::tuple<Flow, int, Flow> ForLoop(InternalExecutionContextPtr aContext, Flow, int aStartIndex, int aEndIndex, int aIncrement, eComparatorType aComparatorType)
	{
		NodeExecutionQueue* previousNodeExecutionQueue = aContext->mNodeExecutionQueue;
		const NodeExecutionData nodeExecutionData = aContext->mNodeData;
		const Node& node = nodeExecutionData.mNodeRef.GetNodeGraph().mNodes[nodeExecutionData.mNodeRef.GetNodeID()];

		for (int i = aStartIndex; Compare(i, aEndIndex, aComparatorType); i += aIncrement)
		{
			NodeExecutionQueue executionQueue(*aContext->mNodeExecutor);

			aContext->mNodeExecutionQueue = &executionQueue;

			SetOutputValues(std::tuple{ Flow(true), i }, node.mOutputPins, *aContext);

			executionQueue.Execute();

			aContext->mNodeData = nodeExecutionData;

		}

		aContext->mNodeExecutionQueue = previousNodeExecutionQueue;

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
			aContext->mClass->mEventGraph.BindNodeToEvent(aContext->mNodeData.mNodeRef.GetNodeID());
		}
		aState.mValue.time += aContext->mExecutionContext->mDeltaTime;
		if (aState.mValue.time > aDuration)
		{
			aState.mValue.time = 0.f;
			aContext->mClass->mEventGraph.UnbindNodeFromEvent(aContext->mNodeData.mNodeRef.GetNodeID());
			return Flow(true);
		}
		return Flow(false);
	}

	struct Emil
	{
		float a = 4.f;
		int b = 2;
		std::vector<std::string> s;
	};

	struct Emil2
	{
		Emil e;
	};

	FLY_VALUETYPE(Emil);

	FLY_MEMBER(Emil::a);
	FLY_MEMBER(Emil::s);

	FLY_VALUETYPE(Emil2);
	FLY_MEMBER(Emil2::e);

	std::tuple<Emil> EmilTest(InternalExecutionContextPtr, Emil e)
	{
		return e;
	}

	void EmilTest2(Emil2, Emil2, float**)
	{
	}

	FLY_FUNCTION(EmilTest);
	FLY_FUNCTION(EmilTest2);

	FLY_FUNCTION(BeginPlay, Directory{ "Events" }, Event{}, Pure{});
	FLY_FUNCTION(Tick, Directory{ "Events" }, Event{}, OutputNames{ "Flow", "Delta Time" }, Pure{});
	FLY_FUNCTION(EndPlay, Directory{ "Events" }, Event{}, Pure{});

	FLY_FUNCTION(ForLoop, Directory{ "Execution" }, InputNames{ "Flow", "Start Index", "End Index", "Increment", "Comparator" }, OutputNames{ "Flow", "Index", "Complete" }, DefaultValues{ Flow(true), 0, 0, 1, eComparatorType::Less }, Pure{});
	FLY_FUNCTION(Branch, Directory{ "Execution" }, InputNames{ "Flow", "Condition" }, OutputNames{ "True", "False" }, Pure{});
	FLY_FUNCTION(Sequence, Directory{ "Execution" }, InputNames{ "Flow" }, Pure{});
	FLY_FUNCTION(FlipFlop, Directory{ "Execution" }, OutputNames{ "Flip", "Flop" }, Pure{});
	FLY_FUNCTION(Delay, Directory{ "Execution" }, InputNames{ "Flow", "Duration", "Reset On Flow" }, DefaultValues{ Flow(false), 1.f, false }, AutoTick{}, Pure{});
}
