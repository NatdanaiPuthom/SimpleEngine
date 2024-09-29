#include "Fly.hpp"
#include "FlyClass.hpp"
#include "Node/FlyNodeTypeManager.hpp"
#include "Pin/FlyPinTypeManager.hpp"
#include "Node/FlyNodeTypeRegistry.hpp"
#include "FlyInternal.hpp"
#include "Utilities/FlyLinker.hpp"
#include "Command/FlyCommandTracker.hpp"
#include "FlyFoundation.hpp"
#include "Serialization/FlySerializer.hpp"

namespace FLY_NAMESPACE
{

	namespace Internal
	{

		static const Pin& GetPin(const PinID aPinID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mPins.at(aPinID);
		}

		static Pin& GetPin(const PinID aPinID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mPins.at(aPinID);
		}

		PinType& GetPinType(const Pin& aPin)
		{
			return Global::GetPinTypeManager().GetPinType(aPin.mTypeID);
		}

		static Node& GetNode(const NodeID aNodeID, NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mNodes.at(aNodeID);
		}

		const Node& GetNode(const NodeID aNodeID, const NodeGraph& aNodeGraph)
		{
			return aNodeGraph.mNodes.at(aNodeID);
		}

		static const NodeType& GetNodeType(const Node& aNode)
		{
			return Global::GetNodeTypeManager().GetNodeType(aNode.mTypeID);
		}
	}

	void SaveClass(const ClassFacade aClassFacade, const std::string_view aSavePath)
	{
		ScriptLoader::SaveClass(aClassFacade.GetClass(), aSavePath);
	}

	void LoadAllClasses(const std::string_view aFilePath)
	{
		ScriptLoader::LoadAllClasses(aFilePath);
	}

	void SaveCustomEvents(const std::string_view aFilePath)
	{
		ScriptLoader::SaveCustomEvents(aFilePath);
	}

	ClassFacade CreateClass(const DataTypeFacade aTargetFacade, const std::string_view aName, const std::string_view aSavePath)
	{
		Class& createdClass = Global::GetFoundation().CreateClass(aTargetFacade.GetID(), aName);
		SaveClass(ClassFacade(createdClass), aSavePath);
		return ClassFacade(createdClass);
	}

	ClassFacade CreateClassWithoutTarget(const std::string_view aName, const std::string_view aSavePath)
	{
		return CreateClass(DataTypeFacade(GetDataTypeID<None*>()), aName, aSavePath);
	}

	void SetClassName(const ClassFacade aClassFacade, const std::string_view aName)
	{
		aClassFacade.GetClass().mName = aName;
	}

	ClassFacade FindClassByName(const std::string_view aName)
	{
		auto& classes = Global::GetFoundation().mClasses;

		for (HeapObject<Class, false>& c : classes)
		{
			if (ClassFacade(*c).GetName() == aName)
			{
				return ClassFacade(*c);
			}
		}

		return ClassFacade();
	}

	ClassInstanceFacade CreateClassInstance(const ClassFacade aClassFacade)
	{
		return ClassInstanceFacade(aClassFacade.GetClass().CreateClassInstance());
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
			Global::GetDataTypeManager().EditData(variable.mDataTypeID, variableInstance.mDefaultValueDataPtr);
		}

	}

	NodeFacade CreateNode(NodeGraphFacade aNodeGraphFacade, const NodeTypeFacade aNodeTypeFacade, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNode(aNodeGraphFacade.GetVariant(), aNodeTypeFacade.GetID(), aPosition, aCommandTracker);
		return NodeFacade(nodeID, aNodeGraphFacade);
	}

	NodeFacade CreateNode(NodeGraphFacade aNodeGraphFacade, std::string_view aName, bool& aSuccess, const Vec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
	{
		const NodeID nodeID = Internal::CreateNode(aNodeGraphFacade.GetVariant(), aName, aSuccess, aPosition, aCreateIfNameNotFound, aCommandTracker);
		return NodeFacade(nodeID, aNodeGraphFacade);
	}

	NodeFacade CreateNodeAutoLink(NodeGraphFacade aNodeGraphFacade, const NodeTypeFacade aNodeTypeFacade, const PinID aConnection, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Node + Auto Link");
		}

		NodeGraph& nodeGraph = aNodeGraphFacade.GetNodeGraph();

		const NodeID createdNodeID = Internal::CreateNode(aNodeGraphFacade.GetVariant(), aNodeTypeFacade.GetID(), aPosition, aCommandTracker);

		const Pin& createdFromPin = nodeGraph.mPins.at(aConnection);
		const PinType& pinType = Global::GetPinTypeManager().GetPinType(createdFromPin.mTypeID);
		const Node& createdNode = nodeGraph.mNodes.at(createdNodeID);

		const std::vector<PinID>& pinIDs = SelectByFlowType(pinType.mFlowType, createdNode.mOutputPins, createdNode.mInputPins);

		for (const PinID pinID : pinIDs)
		{
			if (Internal::TryCreateLink(nodeGraph, pinID, aConnection, aCommandTracker) != InvalidID<LinkID>())
			{
				break;
			}
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeFacade(createdNodeID, aNodeGraphFacade);
	}

	NodeFacade CreateGetterNode(NodeGraphFacade aNodeGraphFacade, const VariableFacade aVariableFacade, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Getter Node");
		}

		const Variable& variable = aVariableFacade.GetClass().mStruct.mVariables.at(aVariableFacade.GetID());

		const NodeID nodeID = Internal::CreateGetterNode(aNodeGraphFacade.GetNodeGraph(), variable.mDataTypeID, aCommandTracker);
		Internal::SetNodePosition(nodeID, aPosition, aNodeGraphFacade.GetNodeGraph(), aCommandTracker);
		Internal::BindVariable(aVariableFacade.GetClass(), CreateContextualNodeRef(nodeID, aNodeGraphFacade.GetNodeGraph()), aVariableFacade.GetID(), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeFacade(nodeID, aNodeGraphFacade);
	}

	NodeFacade CreateSetterNode(NodeGraphFacade aNodeGraphFacade, const VariableFacade aVariableFacade, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Setter Node");
		}

		NodeGraph& nodeGraph = aNodeGraphFacade.GetNodeGraph();

		const Variable& variable = aVariableFacade.GetClass().mStruct.mVariables.at(aVariableFacade.GetID());

		const NodeID nodeID = Internal::CreateSetterNode(nodeGraph, variable.mDataTypeID, aCommandTracker);
		Internal::SetNodePosition(nodeID, aPosition, nodeGraph, aCommandTracker);
		Internal::BindVariable(aVariableFacade.GetClass(), CreateContextualNodeRef(nodeID, nodeGraph), aVariableFacade.GetID(), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeFacade(nodeID, aNodeGraphFacade);
	}

	LinkFacade TryCreateLink(const PinFacade aPinFacade1, const PinFacade aPinFacade2, NodeGraphFacade aNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		const LinkID createdLinkID = Internal::TryCreateLink(aNodeGraphFacade.GetNodeGraph(), aPinFacade1.GetID(), aPinFacade2.GetID(), aCommandTracker);
		return LinkFacade(createdLinkID, aNodeGraphFacade);
	}

	void DestroyLink(const LinkFacade aLinkFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker)
	{
		Internal::DestroyLink(aNodeGraphFacade.GetNodeGraph(), aLinkFacade.GetID(), aCommandTracker);
	}

	void DestroyLinksByPin(const PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		Internal::DestroyLinksByPin(aNodeGraphFacade.GetNodeGraph(), aPinFacade.GetID(), aCommandTracker);
	}

	void DestroyLinksByNode(NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker)
	{
		std::vector<LinkID> linkIDs = ScriptLinker::GetLinkIDsByNode(aNodeGraphFacade.GetNodeGraph(), aNodeFacade.GetID());

		DestroySelection({}, linkIDs, aNodeGraphFacade, aCommandTracker);
	}

	void DestroyNode(const NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		return Internal::DestroyNode(aNodeGraphFacade.GetNodeGraph(), aNodeFacade.GetID(), aCommandTracker);
	}

	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Selection");
		}

		for (const LinkID linkID : aLinkIDs)
		{
			DestroyLink(LinkFacade(linkID, aNodeGraphFacade), aNodeGraphFacade, aCommandTracker);
		}

		for (const NodeID nodeID : aNodeIDs)
		{
			DestroyNode(NodeFacade(nodeID, aNodeGraphFacade), aNodeGraphFacade, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* const aCommandTracker)
	{
		Internal::DestroyNodes(aNodeRefs, aCommandTracker);
	}

	void SetNodePosition(const NodeFacade aNodeFacade, Vec2 aPosition, NodeGraphFacade aNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		const Vec2 oldPos = Internal::GetNode(aNodeFacade.GetID(), aNodeGraphFacade.GetNodeGraph()).mPosition;
		SetNodePosition(aNodeFacade, aPosition, oldPos, aNodeGraphFacade, aCommandTracker);
	}

	void SetNodePosition(const NodeFacade aNodeFacade, const Vec2 aPosition, const Vec2 aOldPosition, NodeGraphFacade aNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		Internal::SetNodePosition(aNodeFacade.GetID(), aPosition, aOldPosition, aNodeGraphFacade.GetNodeGraph(), aCommandTracker);
	}

	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraphFacade aNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		if (aDragData.empty())
		{
			return;
		}
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Drag nodes");
		}

		for (const auto& [mNodeID, dragData] : aDragData)
		{
			Internal::SetNodePosition(mNodeID, dragData.mEndPos, dragData.mStartPos, aNodeGraphFacade.GetNodeGraph(), aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	VariableFacade CreateVariable(const ClassFacade aClassFacade, const DataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		const VarID varID = Internal::CreateVariable(aClassFacade.GetClass(), aDataTypeFacade.GetID(), aCommandTracker);
		return VariableFacade(varID, aClassFacade);
	}

	void DestroyVariable(const VariableFacade aVariableFacade, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Variable + Connected Nodes");
		}

		struct DestroyVariableData
		{
			VarID mVarID = InvalidID<VarID>();
			Class* mClass = nullptr;
		} data;

		data.mVarID = aVariableFacade.GetID();
		data.mClass = &aVariableFacade.GetClass();

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				bool mIsDestroyed = aCommandType == eCommandType::Do;
				data.mClass->mStruct.mVariables.at(data.mVarID).mIsDestroyed = mIsDestroyed;
			};

		if (!aCommandTracker)
		{
			commandFunction(eCommandType::Do);
		}
		else
		{
			aCommandTracker->DoCommand(Command(commandFunction, "Destroy Variable"));
		}

		Internal::DestroyNodes(Internal::GetNodeRefsByVariableRef(VariableRef(aVariableFacade.GetID(), aVariableFacade.GetClass())), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void EditVariableDefaultValue(const VariableFacade aVariableFacade, CommandTracker*)
	{
		assert(aVariableFacade);
		Variable& variable = aVariableFacade.GetClass().mStruct.mVariables.at(aVariableFacade.GetID());

		if (Global::GetDataTypeManager().EditData(variable.mDataTypeID, variable.mDefaultValueDataPtr))
		{

		}
	}

	void SetVariableDataType(const VariableFacade aVariableFacade, const DataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		Internal::SetVariableDataType(aVariableFacade.GetClass(), aVariableFacade.GetID(), aDataTypeFacade.GetID(), aCommandTracker);
	}

	void SetVariableName(const VariableFacade aVariableFacade, const std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		Variable& variable = aVariableFacade.GetClass().mStruct.mVariables.at(aVariableFacade.GetID());

		variable.mName = aName;
	}

	void DestroyVariableNodes(const VariableFacade aVariableFacade, ClassFacade aClassFacade, CommandTracker* const aCommandTracker)
	{
		Internal::DestroyNodes(Internal::GetNodeRefsByVariableRef(VariableRef(aVariableFacade.GetID(), aClassFacade.GetClass())), aCommandTracker);
	}

	void EditPin(const PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker)
	{
		NodeGraph& nodeGraph = aNodeGraphFacade.GetNodeGraph();
		Pin& pin = nodeGraph.mPins.at(aPinFacade.GetID());
		const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

		DataTypeManager& dataTypeManager = Global::GetDataTypeManager();

		const void* const copyDataPtr = [aCommandTracker, &dataTypeManager, &pinType, &pin]() -> const void*
			{
				return aCommandTracker != nullptr ? dataTypeManager.AllocateData(pinType.mDataTypeID, Global::Internal::GetFrameMemoryArena(), pin.mDataPtr) : nullptr;
			}();

		const bool wasEdited = dataTypeManager.EditData(pinType.mDataTypeID, pin.mDataPtr);

		if (!wasEdited || !aCommandTracker)
		{
			return;
		}


		void* const previousDataPtr = Global::GetDataTypeManager().AllocateData(pinType.mDataTypeID, Global::Internal::GetEditMemoryArena(), copyDataPtr);

		struct EditPinData
		{
			PinID mPinID = InvalidID<PinID>();
			void* mPreviousDataPtr = nullptr;
			NodeGraph* mNodeGraph = nullptr;
		} data;

		data.mPinID = aPinFacade.GetID();
		data.mPreviousDataPtr = previousDataPtr;
		data.mNodeGraph = &nodeGraph;

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				if (aCommandType == eCommandType::Do)
				{
					Pin& pin = data.mNodeGraph->mPins.at(data.mPinID);
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

					Global::GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, data.mPreviousDataPtr);
				}
				else
				{
					Pin& pin = data.mNodeGraph->mPins.at(data.mPinID);
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

					Global::GetDataTypeManager().SwapData(pinType.mDataTypeID, pin.mDataPtr, data.mPreviousDataPtr);
				}
			};

		aCommandTracker->RegisterCommand(Command(commandFunction, "Edit Pin"));
	}

	void SplitPin(const PinFacade aPinFacade, NodeGraphFacade aNodeGraphFacade, CommandTracker* aCommandTracker)
	{
		aCommandTracker;
		NodeGraph& nodeGraph = aNodeGraphFacade.GetNodeGraph();
		const Pin& pin = nodeGraph.mPins.at(aPinFacade.GetID());

		if (!pin.mConnectedPinIDs.empty())
		{
			return;
		}

		const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);

		const DataType* pinDataType = Global::GetDataTypeManager().Find(pinType.mDataTypeID);

		if (pinDataType == nullptr)
		{
			return;
		}
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

	bool IsNodeReplacable(const NodeFacade aNodeFacade, const NodeGraphFacade aNodeGraphFacade)
	{
		const Node& node = Internal::GetNode(aNodeFacade.GetID(), aNodeGraphFacade.GetNodeGraph());
		const NodeType& nodeType = Internal::GetNodeType(node);

		return nodeType.mNodeRecipe.mOperatorTrait != eNodeOperatorTrait::None;
	}

	bool IsPinReplacable(const PinFacade aPinFacade, const NodeGraphFacade aNodeGraphFacade)
	{
		return IsNodeReplacable(NodeFacade(aPinFacade.GetNodeID(), aNodeGraphFacade), aNodeGraphFacade);
	}

	void ReplaceTemplateNode(const PinFacade aReplacedPinFacade, NodeGraphFacade aNodeGraphFacade, const DataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceTemplateNode(aNodeGraphFacade.GetNodeGraph(), aReplacedPinFacade.GetNodeID(), aDataTypeFacade.GetID(), aCommandTracker);
	}

	void ReplaceTemplateNode(const NodeFacade aNodeFacade, NodeGraphFacade aNodeGraphFacade, const DataTypeFacade aDataTypeFacade, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceTemplateNode(aNodeGraphFacade.GetNodeGraph(), aNodeFacade.GetID(), aDataTypeFacade.GetID(), aCommandTracker);
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

		const Node& node = Internal::GetNode(aNodeFacade.GetID(), aNodeGraphFacade.GetNodeGraph());
		const NodeType& nodeType = Internal::GetNodeType(node);
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
				const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(copiedFromNodeGraph, copiedInputPinID, nodeGraphCopy, createdNodeID);
				assert(createdInputPinID != InvalidID<PinID>());

				const std::vector<LinkID> connectedLinks = ScriptLinker::GetLinkIDsByPin(copiedFromNodeGraph, copiedInputPinID);

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
					const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(copiedFromNodeGraph, connectedOutputPinID, nodeGraphCopy, newConnectedNodeID);

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

				const PinID sourcePinID = ScriptLinker::GetOpposingPinID(targetNodeGraph, createdInputPinID, nodeGraphCopy, sourceNodeID);
				const Pin& sourcePin = nodeGraphCopy.mPins.at(sourcePinID);
				dataTypeManager.CopyData(createdInputPinType.mDataTypeID, createdInputPin.mDataPtr, sourcePin.mDataPtr);
			}
		}

		for (const Link& link : nodeGraphCopy.mLinks)
		{
			const Pin& inputPin = nodeGraphCopy.mPins[link.mInputPinID];
			const Pin& outputPin = nodeGraphCopy.mPins[link.mOutputPinID];
			const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(nodeGraphCopy, link.mInputPinID, targetNodeGraph, nodeConverter.at(inputPin.mNodeID));
			const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(nodeGraphCopy, link.mOutputPinID, targetNodeGraph, nodeConverter.at(outputPin.mNodeID));
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

	FunctionFacade CreateMemberFunction(const std::string_view aName, ClassFacade aClassFacade)
	{
		const FunctionID id = Internal::CreateFunction(aName);
		aClassFacade.GetClass().BindFunction(id);

		return FunctionFacade(id);
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

	void SetFunctionName(const FunctionFacade aFunctionFacade, const std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
	{
		Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionFacade.GetID());

		function.mName = aName;
	}

	void BeginFrame()
	{
		Global::Internal::GetFrameMemoryArena().Clear();
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



	std::vector<PinFacade> GetNonConnectedInputPinFacades(const NodeGraphFacade aNodeGraphFacade)
	{
		return GetPinFacadesFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Global::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Input;
			},
			aNodeGraphFacade
		);
	}

	std::vector<PinFacade> GetNonConnectedOutputPinFacades(const NodeGraphFacade aNodeGraphFacade)
	{
		return GetPinFacadesFiltered([](const Pin& aPin)-> bool
			{
				return aPin.mConnectedPinIDs.empty() && Global::GetPinTypeManager().GetPinType(aPin.mTypeID).mFlowType == eFlowType::Output;
			},
			aNodeGraphFacade
		);
	}

	std::vector<PinFacade> GetNonConnectedPinFacadesByFlowType(const NodeGraphFacade aNodeGraphFacade, const eFlowType aFlowType)
	{
		switch (aFlowType)
		{
		case eFlowType::Input:
			return GetNonConnectedInputPinFacades(aNodeGraphFacade);
			break;
		case eFlowType::Output:
			return GetNonConnectedOutputPinFacades(aNodeGraphFacade);
			break;
		default:
			break;
		}
		return std::vector<PinFacade>();
	}

	std::vector<PinFacade> GetNonConnectedPinFacadesByFlowTypeAndDataType(const NodeGraphFacade aNodeGraphFacade, const eFlowType aFlowType, const DataTypeFacade aDataTypeFacade)
	{
		return GetPinFacadesFiltered(
			[aFlowType, dataTypeID = aDataTypeFacade.GetID()](const Pin& aPin) -> bool
			{
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(aPin.mTypeID);
				return aPin.mConnectedPinIDs.empty() && pinType.mFlowType == aFlowType && pinType.mDataTypeID == dataTypeID;
			},
			aNodeGraphFacade
		);
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

	std::vector<NodeFacade> GetNodes(const NodeGraphFacade aNodeGraphFacade, bool aIncludeDestroyed)
	{
		const std::vector<Node>& nodes = aNodeGraphFacade.GetNodeGraph().mNodes;
		std::vector<NodeFacade> views;

		views.reserve(nodes.size());

		for (NodeID mNodeID = 0; mNodeID < nodes.size(); ++mNodeID)
		{
			if (!aIncludeDestroyed)
			{
				const Node& node = nodes.at(mNodeID);
				if (node.mIsDestroyed)
				{
					continue;
				}
			}
			views.push_back(NodeFacade(mNodeID, aNodeGraphFacade));
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
		std::vector<DataTypeFacade> views;
		views.reserve(dataTypes.size());

		for (const auto& [dataTypeID, dataType] : dataTypes)
		{
			views.push_back(DataTypeFacade(dataTypeID));
		}

		return views;
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

