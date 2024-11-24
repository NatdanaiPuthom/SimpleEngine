#include "Fly.hpp"
#include "FlyClass.hpp"
#include "Node/FlyNodeTypeManager.hpp"
#include "Pin/FlyPinTypeManager.hpp"
#include "Node/FlyNodeTypeRegistry.hpp"
#include "FlyInternal.hpp"
#include "Command/FlyCommandTracker.hpp"
#include "FlyFoundation.hpp"
#include "Serialization/FlySerializer.hpp"
#include "DataType/FlyPtrValueVariant.hpp"

namespace FLY_NAMESPACE
{

	void InitializeFoundation()
	{
		Foundation::GetInstance().Initialize();
	}

	void DestroyFoundation()
	{
		Foundation::GetInstance().Destroy();
	}

	void LoadAllClasses(const std::string_view aFilePath)
	{
		Internal::LoadAllClasses(aFilePath);
	}

	void SaveCustomEvents(const std::string_view aFilePath)
	{
		Internal::SaveCustomEvents(aFilePath);
	}

	ClassFacade CreateClass(const DataTypeFacade aTargetFacade, const std::string_view aName, const std::string_view aSavePath)
	{
		Class& createdClass = Global::GetFoundation().CreateClass(aTargetFacade.GetID(), aName);
		Internal::SaveClass(createdClass, aSavePath);
		return ClassFacade(createdClass);
	}

	ClassFacade CreateClassWithoutTarget(const std::string_view aName, const std::string_view aSavePath)
	{
		return CreateClass(DataTypeFacade(GetDataTypeID<None*>()), aName, aSavePath);
	}

	ClassFacade FindClassByName(const std::string_view aName)
	{
		auto& classes = Global::GetFoundation().mClasses;

		for (HeapObject<Class>& c : classes)
		{
			if (ClassFacade(*c).GetName() == aName)
			{
				return ClassFacade(*c);
			}
		}

		return ClassFacade();
	}

	void DestroyClassInstance(ClassInstanceFacade aClassInstanceFacade)
	{
		aClassInstanceFacade.GetClassInstance().mClass->DestroyClassInstance(aClassInstanceFacade.GetClassInstance());
	}

	void EditClassInstanceVariableDefaultValue(ClassInstanceFacade aClassInstanceFacade)
	{
		ClassInstance& classInstance = aClassInstanceFacade.GetClassInstance();
		const std::vector<Variable>& variables = classInstance.mStructInstance.mStruct->mVariables;
		std::vector<VariableInstance>& variableInstances = classInstance.mStructInstance.mVariableInstances;

		assert(variables.size() == variableInstances.size());

		for (size_t i = 0; i < variableInstances.size(); ++i)
		{
			const Variable& variable = variables[i];
			VariableInstance& variableInstance = variableInstances[i];
			Global::GetDataTypeManager().ViewAndEditData(variable.mDataTypeID, variableInstance.mDefaultValueDataPtr);
		}

	}

	void EditVariableDefaultValue(const VariableFacade aVariableFacade, CommandTracker*)
	{
		assert(aVariableFacade);
		Variable& variable = aVariableFacade.GetClass().mStruct.mVariables.at(aVariableFacade.GetID());

		Global::GetDataTypeManager().ViewAndEditData(variable.mDataTypeID, variable.mDefaultValueDataPtr);
	}

	bool HasPinAnyConnectedLinks(const PinFacade aPinFacade)
	{
		return !aPinFacade.GetConnectedPinIDs().empty();
	}

	bool HasNodeAnyConnectedLinks(const NodeFacade aNodeFacade, const NodeGraphFacade aNodeGraphFacade)
	{
		const Node& node = aNodeGraphFacade.GetNodeGraph().mNodes.at(aNodeFacade.GetID());

		auto hasConnectedLink = [](const std::vector<PinID>& aPinIDs, const NodeGraph& aNodeGraph) -> bool
			{
				for (const PinID pinID : aPinIDs)
				{
					const Pin& pin = aNodeGraph.mPins.at(pinID);
					if (!pin.mConnectedPinIDs.empty())
					{
						return true;
					}
				}

				return false;
			};

		return hasConnectedLink(node.mInputPins, aNodeGraphFacade.GetNodeGraph()) || hasConnectedLink(node.mOutputPins, aNodeGraphFacade.GetNodeGraph());
	}

	std::vector<DataTypeFacade> GetReplacableDataTypes(const PinFacade aPinFacade, const NodeGraphFacade aNodeGraphFacade)
	{
		assert(aPinFacade && "Facade is not valid");
		assert(aNodeGraphFacade && "Facade is not valid");

		return GetReplacableDataTypes(NodeFacade(aPinFacade.GetNodeID(), aNodeGraphFacade), aNodeGraphFacade);
	}

	std::vector<DataTypeFacade> GetReplacableDataTypes(const NodeFacade aNodeFacade, const NodeGraphFacade aNodeGraphFacade)
	{
		assert(aNodeFacade && "Facade is not valid");
		assert(aNodeGraphFacade && "Facade is not valid");

		const NodeType& nodeType = Internal::GetNodeType(aNodeFacade.GetID(), aNodeGraphFacade.GetNodeGraph());
		std::vector<DataTypeFacade> dataTypeFacades;

		auto& dataTypes = Global::GetNodeTypeManager().GetTemplateMapByOperator(nodeType.mNodeRecipe.mOperatorTrait);
		dataTypeFacades.reserve(dataTypes.size());
		for (auto& [dataTypeID, nodeTypeID] : dataTypes)
		{
			dataTypeFacades.push_back(DataTypeFacade(dataTypeID));
		}

		return dataTypeFacades;
	}

	void SetPinTypeName(const PinTypeFacade aPinTypeFacade, std::string_view aName)
	{
		PinType& pinType = Global::GetPinTypeManager().GetPinType(aPinTypeFacade.GetID());

		pinType.mName = aName;
	}

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraphFacade aCopiedFromNodeGraphFacade)
	{
		if (aNodeIDs.empty())
		{
			return;
		}
		const NodeGraph& copiedFromNodeGraph = aCopiedFromNodeGraphFacade.GetNodeGraph();
		EventGraph& eventGraphCopy = Global::Internal::GetNodeGraphCopy();
		eventGraphCopy = EventGraph{};

		NodeGraph& nodeGraphCopy = eventGraphCopy.mNodeGraph;

		std::vector<NodeID> createdNodeIDs;
		std::unordered_map<NodeID, NodeID> nodeConverter;
		createdNodeIDs.reserve(aNodeIDs.size());

		Vec2 avgPos;
		for (const NodeID nodeID : aNodeIDs)
		{
			const Node& node = copiedFromNodeGraph.mNodes.at(nodeID);

			const NodeID createdNodeID = Internal::CreateNode(&eventGraphCopy, node.mTypeID, node.mPosition, nullptr);
			avgPos += node.mPosition;

			createdNodeIDs.push_back(createdNodeID);
			nodeConverter.emplace(nodeID, createdNodeID);
		}

		avgPos /= static_cast<float>(aNodeIDs.size());

		for (Node& node : nodeGraphCopy.mNodes)
		{
			node.mPosition = node.mPosition - avgPos;
		}

		// Create links
		for (NodeID copiedNodeID : aNodeIDs)
		{
			const Node& copiedNode = copiedFromNodeGraph.mNodes.at(copiedNodeID);
			for (const PinID copiedInputPinID : copiedNode.mInputPins)
			{
				const Pin& copiedInputPin = Internal::GetPin(copiedInputPinID, copiedFromNodeGraph);
				const PinType& copiedInputPinType = Global::GetPinTypeManager().GetPinType(copiedInputPin.mTypeID);

				const NodeID createdNodeID = nodeConverter.at(copiedNodeID);
				const PinID createdInputPinID = Internal::GetOpposingPinID(copiedFromNodeGraph, copiedInputPinID, nodeGraphCopy, createdNodeID);
				assert(createdInputPinID != InvalidID<PinID>());

				const std::vector<LinkID> connectedLinks = Internal::GetLinkIDsByPin(copiedFromNodeGraph, copiedInputPinID);

				for (const LinkID connectedLinkID : connectedLinks)
				{
					const Link& connectedLink = copiedFromNodeGraph.mLinks[connectedLinkID];

					const PinID connectedOutputPinID = connectedLink.mOutputPinID;
					const Pin& connectedOutputPin = Internal::GetPin(connectedOutputPinID, copiedFromNodeGraph);

					auto it = nodeConverter.find(connectedOutputPin.mNodeID);
					if (it == nodeConverter.end())
					{
						continue;
					}
					const NodeID newConnectedNodeID = it->second;
					const PinID createdOutputPinID = Internal::GetOpposingPinID(copiedFromNodeGraph, connectedOutputPinID, nodeGraphCopy, newConnectedNodeID);

					Internal::TryCreateLink(nodeGraphCopy, createdInputPinID, createdOutputPinID, nullptr);
				}

				Pin& createdInputPin = Internal::GetPin(createdInputPinID, nodeGraphCopy);

				Global::GetDataTypeManager().CopyData(copiedInputPinType.mDataTypeID, createdInputPin.mDataPtr, copiedInputPin.mDataPtr);
			}
		}
	}

	void PasteCopyBuffer(const Vec2 aPosition, NodeGraphFacade aTargetNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		const NodeGraph& nodeGraphCopy = Global::Internal::GetNodeGraphCopy().mNodeGraph;

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Paste Nodes");
		}

		NodeGraph& targetNodeGraph = aTargetNodeGraphFacade.GetNodeGraph();

		std::unordered_map<NodeID, NodeID> nodeConverter;

		for (NodeID sourceNodeID = 0; sourceNodeID < nodeGraphCopy.mNodes.size(); sourceNodeID++)
		{
			const Node& node = nodeGraphCopy.mNodes.at(sourceNodeID);
			const NodeID createdNodeID = Internal::CreateNode(aTargetNodeGraphFacade.GetVariant(), node.mTypeID, aPosition + node.mPosition, aCommandTracker);
			nodeConverter.emplace(sourceNodeID, createdNodeID);

			const Node& createdNode = Internal::GetNode(createdNodeID, targetNodeGraph);
			DataTypeManager& dataTypeManager = Global::GetDataTypeManager();
			for (const PinID createdInputPinID : createdNode.mInputPins)
			{
				Pin& createdInputPin = Internal::GetPin(createdInputPinID, targetNodeGraph);
				const PinType& createdInputPinType = Global::GetPinTypeManager().GetPinType(createdInputPin.mTypeID);

				const PinID sourcePinID = Internal::GetOpposingPinID(targetNodeGraph, createdInputPinID, nodeGraphCopy, sourceNodeID);
				const Pin& sourcePin = nodeGraphCopy.mPins.at(sourcePinID);
				dataTypeManager.CopyData(createdInputPinType.mDataTypeID, createdInputPin.mDataPtr, sourcePin.mDataPtr);
			}
		}

		for (const Link& link : nodeGraphCopy.mLinks)
		{
			const Pin& inputPin = nodeGraphCopy.mPins[link.mInputPinID];
			const Pin& outputPin = nodeGraphCopy.mPins[link.mOutputPinID];
			const PinID createdInputPinID = Internal::GetOpposingPinID(nodeGraphCopy, link.mInputPinID, targetNodeGraph, nodeConverter.at(inputPin.mNodeID));
			const PinID createdOutputPinID = Internal::GetOpposingPinID(nodeGraphCopy, link.mOutputPinID, targetNodeGraph, nodeConverter.at(outputPin.mNodeID));
			const LinkID createdLinkID = Internal::TryCreateLink(targetNodeGraph, createdInputPinID, createdOutputPinID, aCommandTracker);

			assert(createdLinkID != InvalidID<LinkID>());
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	CustomEventFacade CreateCustomEvent(const std::string_view aName)
	{
		return CustomEventFacade(Internal::CreateCustomEvent(aName));
	}

	void AddPinToCustomEvent(const CustomEventFacade aCustomEventFacade, const DataTypeFacade aDataTypeFacade, std::string_view aPinName, CommandTracker* const aCommandTracker)
	{
		Internal::AddPinToCustomEvent(aCustomEventFacade.GetID(), aDataTypeFacade.GetID(), aPinName, aCommandTracker);
	}

	void SetPinDataTypeAtIndexCustomEvent(const CustomEventFacade aCustomEventFacade, const DataTypeFacade aDataTypeFacade, const size_t aIndex, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexCustomEvent(aCustomEventFacade.GetID(), aDataTypeFacade.GetID(), aIndex, aCommandTracker);
	}

	void SetPinNameAtIndexCustomEvent(const CustomEventFacade aCustomEventFacade, const std::string_view aName, const size_t aIndex, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinNameAtIndexCustomEvent(aCustomEventFacade.GetID(), aName, aIndex, aCommandTracker);
	}

	void DeletePinAtIndexCustomEvent(const CustomEventFacade aCustomEventFacade, const size_t aIndex, CommandTracker* const aCommandTracker)
	{
		Internal::DeletePinAtIndexCustomEvent(aCustomEventFacade.GetID(), aIndex, aCommandTracker);
	}

	void SetCustomEventName(const CustomEventFacade aCustomEventFacade, std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventFacade.GetID());
		NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		NodeType& callerNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetCallerTypeID());

		const std::string nameDirectory = Global::GetNodeTypeManager().GetNameDirectory(customEvent.GetExecutorTypeID());
		executorNodeType.mNodeRecipe.mName = nameDirectory + std::string(aName);
		callerNodeType.mNodeRecipe.mName = nameDirectory + "Call " + std::string(aName);
	}

	FunctionFacade CreateGlobalFunction(const std::string_view aName)
	{
		return FunctionFacade(Internal::CreateFunction(aName));
	}

	void AddPinToFunction(const FunctionFacade aFunctionFacade, const DataTypeFacade aDataTypeFacade, const eFlowType aFlowType, std::string_view aPinName, CommandTracker* const aCommandTracker)
	{
		Internal::AddPinToFunction(aFunctionFacade.GetID(), aDataTypeFacade.GetID(), aFlowType, aPinName, aCommandTracker);
	}

	void SetPinDataTypeAtIndexFunction(const FunctionFacade aFunctionFacade, const DataTypeFacade aDataTypeFacade, const size_t aIndex, const eFlowType aFlowType, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinDataTypeAtIndexFunction(aFunctionFacade.GetID(), aDataTypeFacade.GetID(), aIndex, aFlowType, aCommandTracker);
	}

	void SetPinNameAtIndexFunction(const FunctionFacade aFunctionFacade, const std::string_view aName, const size_t aIndex, const eFlowType aFlowType, CommandTracker* const aCommandTracker)
	{
		Internal::SetPinNameAtIndexFunction(aFunctionFacade.GetID(), aName, aIndex, aFlowType, aCommandTracker);
	}

	void DeletePinAtIndexFunction(const FunctionFacade aFunctionFacade, const size_t aIndex, const eFlowType aFlowType, CommandTracker* const aCommandTracker)
	{
		Internal::DeletePinAtIndexFunction(aFunctionFacade.GetID(), aIndex, aFlowType, aCommandTracker);
	}

	void BeginFrame(CommandTracker* const aCommandTracker)
	{
		Internal::BeginFrame(aCommandTracker);
	}

	std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed)
	{
		return Internal::GetInputPins(aNodeGraph, aIncludeDestroyed);
	}

	std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, bool aIncludeDestroyed)
	{
		return Internal::GetOutputPins(aNodeGraph, aIncludeDestroyed);
	}

	template<Predicate<const Pin&> Predicate>
	std::vector<PinFacade> GetPinFacadesFiltered(Predicate&& aPredicate, const NodeGraphFacade& aNodeGraphFacade)
	{
		std::vector<PinFacade> pinFacades;
		const NodeGraph& nodeGraph = aNodeGraphFacade.GetNodeGraph();
		pinFacades.reserve(nodeGraph.mPins.size());

		for (PinID pinID = 0; pinID < nodeGraph.mPins.size(); ++pinID)
		{
			const Pin& pin = nodeGraph.mPins[pinID];
			if (aPredicate(pin))
			{
				pinFacades.push_back(PinFacade(pinID, aNodeGraphFacade));
			}
		}

		return pinFacades;
	}

	VariableFacade GetVariableByNode(const NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade)
	{
		const VariableRef variableRef = Internal::GetVariableRefByNodeRef(GlobalNodeRef(aNodeFacade.GetID(), aNodeGraphFacade.GetNodeGraph()));
		return VariableFacade(variableRef.GetVarID(), ClassFacade(variableRef.GetClass()));
	}

	std::vector<VariableFacade> GetVariables(const ClassFacade aClassFacade, const bool aIncludeDestroyed)
	{
		const std::vector<Variable>& variables = aClassFacade.GetClass().mStruct.mVariables;
		std::vector<VariableFacade> views;

		views.reserve(variables.size());

		for (VarID varID = 0; varID < variables.size(); ++varID)
		{
			if (!aIncludeDestroyed)
			{
				const Variable& variable = variables.at(varID);
				if (variable.mIsDestroyed)
				{
					continue;
				}
			}
			views.push_back(VariableFacade(varID, aClassFacade));
		}
		return views;
	}

	std::vector<LinkFacade> GetLinks(const NodeGraphFacade aNodeGraphFacade, bool aIncludeDestroyed)
	{
		const std::vector<Link>& links = aNodeGraphFacade.GetNodeGraph().mLinks;
		std::vector<LinkFacade> views;

		views.reserve(links.size());

		for (LinkID linkID = 0; linkID < links.size(); ++linkID)
		{
			if (!aIncludeDestroyed)
			{
				const Link& link = aNodeGraphFacade.GetNodeGraph().mLinks.at(linkID);
				if (link.mIsDestroyed)
				{
					continue;
				}
			}
			views.push_back(LinkFacade(linkID, aNodeGraphFacade));
		}
		return views;
	}

	std::vector<DataTypeFacade> GetDataTypes()
	{
		const auto& dataTypes = Global::GetDataTypeManager().GetDataTypes();
		std::vector<DataTypeFacade> dataTypeFacades;
		dataTypeFacades.reserve(dataTypes.size());

		for (const auto& [dataTypeID, dataType] : dataTypes)
		{
			dataTypeFacades.push_back(DataTypeFacade(dataTypeID));
		}

		return dataTypeFacades;
	}

	std::vector<NodeTypeFacade> GetNodeTypes()
	{
		const auto& nodeTypes = Global::GetNodeTypeManager().GetNodeTypes();
		std::vector<NodeTypeFacade> views;
		views.reserve(nodeTypes.size());

		for (NodeTypeID nodeTypeID = 0; nodeTypeID < nodeTypes.size(); ++nodeTypeID)
		{
			views.push_back(NodeTypeFacade(nodeTypeID));
		}

		return views;
	}

	std::vector<FunctionFacade> GetFunctions()
	{
		const auto& mFunctions = Global::GetNodeTypeManager().GetFunctions();
		std::vector<FunctionFacade> views;
		views.reserve(mFunctions.size());

		for (FunctionID functionID = 0; functionID < mFunctions.size(); ++functionID)
		{
			views.push_back(FunctionFacade(functionID));
		}

		return views;
	}

	std::vector<CustomEventFacade> GetCustomEvents()
	{
		const auto& customEvents = Global::GetNodeTypeManager().GetCustomEvents();
		std::vector<CustomEventFacade> customEventFacades;
		customEventFacades.reserve(customEvents.size());

		for (CustomEventID customEventID = 0; customEventID < customEvents.size(); ++customEventID)
		{
			customEventFacades.push_back(CustomEventFacade(customEventID));
		}

		return customEventFacades;
	}

	template<typename FilterFunction>
	std::vector<NodeTypeFacade> GetNodeTypesFiltered(FilterFunction&& aFilter)
	{
		std::vector<NodeTypeFacade> facades;
		const std::vector<NodeType>& nodeTypes = Global::GetNodeTypeManager().GetNodeTypes();
		facades.reserve(nodeTypes.size());
		for (NodeTypeID id = 0; id < nodeTypes.size(); id++)
		{
			if (aFilter(nodeTypes[id]))
			{
				facades.push_back(NodeTypeFacade(id));
			}
		}
		return facades;
	}

	std::vector<NodeTypeFacade> GetNodeTypesFilteredByDataTypeAndFlowType(const DataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		return GetNodeTypesFiltered([aDataTypeID, aFlowType](const NodeType& aNodeType) -> bool
			{
				const std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, aNodeType.mNodeRecipe.mInputPinTypeIDs, aNodeType.mNodeRecipe.mOutputPinTypeIDs);
				for (const PinTypeID pinTypeID : pinTypeIDs)
				{
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pinTypeID);
					if (pinType.mDataTypeID == aDataTypeID)
					{
						return true;
					}
				}
				return false;
			}
		);
	}

	std::vector<NodeTypeFacade> GetNodeTypesFilteredByTrait(const eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
	{
		return GetNodeTypesFiltered([aNodeTrait, aBitOperation](const NodeType& aNodeType) -> bool
			{
				return aBitOperation(aNodeTrait, aNodeType.mNodeRecipe.mTraits);
			}
		);
	}

	std::unordered_map<DataTypeFacade, std::vector<ClassFacade>> GetClasses()
	{
		auto& classes = Foundation::GetInstance().mClasses;

		std::unordered_map<DataTypeFacade, std::vector<ClassFacade>> views;

		for (auto& flyClass : classes)
		{
			views[DataTypeFacade(flyClass->mTargetID)].push_back(ClassFacade(*flyClass));
		}

		return views;
	}

	std::vector<ClassFacade> GetClassesByDataType(DataTypeFacade aDataTypeFacade)
	{
		auto& classes = Foundation::GetInstance().mClasses;

		std::vector<ClassFacade> views;

		for (auto& flyClass : classes)
		{
			if (flyClass->mTargetID == aDataTypeFacade.GetID())
			{
				views.push_back(ClassFacade(*flyClass));
			}
		}

		return views;
	}
}

