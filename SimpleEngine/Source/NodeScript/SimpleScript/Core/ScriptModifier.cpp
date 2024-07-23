#include "ScriptModifier.hpp"
#include "FlyClass.hpp"
#include "Node/NodeTypeManager.hpp"
#include "Pin/PinTypeManager.hpp"
#include "Node/NodeTypeRegistry.hpp"
#include "ScriptInternalModifier.hpp"
#include "ScriptCopyBuffer.hpp"
#include "Utilities/ScriptLinker.hpp"
#include "Command/ScriptCommandTracker.hpp"
#include "ScriptFoundation.hpp"
#include "Serialization/ScriptLoader.hpp"

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

	void SaveClass(const ClassView aClassView, const std::string_view aSavePath)
	{
		ScriptLoader::SaveClass(aClassView.GetClass(), aSavePath);
	}

	ClassView CreateClass(const DataTypeView aTargetView, const std::string_view aName)
	{
		Class& createdClass = Global::GetFoundation().CreateClass(aTargetView.GetID(), aName);
		return ClassView(createdClass);
	}

	ClassView CreateClassWithoutTarget(const std::string_view aName)
	{
		return CreateClass(DataTypeView(GetDataTypeID<None>()), aName);
	}

	void SetClassName(const ClassView aClassView, const std::string_view aName)
	{
		aClassView.GetClass().Name() = aName;
	}

	NodeView CreateNode(NodeGraphView aNodeGraphView, const NodeTypeView aNodeTypeView, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeID nodeID = Internal::CreateNode(aNodeGraphView.GetAs(), aNodeTypeView.GetID(), aPosition, aCommandTracker);
		return NodeView(nodeID, aNodeGraphView.GetNodeGraph());
	}

	NodeView CreateNode(NodeGraphView aNodeGraphView, std::string_view aName, bool& aSuccess, const Vec2 aPosition, CommandTracker* const aCommandTracker, const bool aCreateIfNameNotFound)
	{
		const NodeID nodeID = Internal::CreateNode(aNodeGraphView.GetAs(), aName, aSuccess, aPosition, aCreateIfNameNotFound, aCommandTracker);
		return NodeView(nodeID, aNodeGraphView.GetNodeGraph());
	}

	NodeView CreateNodeAutoLink(NodeGraphView aNodeGraphView, const NodeTypeView aNodeTypeView, const PinID aConnection, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Node + Auto Link");
		}

		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();

		const NodeID createdNodeID = Internal::CreateNode(&nodeGraph, aNodeTypeView.GetID(), aPosition, aCommandTracker);

		const Pin& createdFromPin = nodeGraph.mPins.at(aConnection);
		const PinType& pinType = Global::GetPinTypeManager().GetPinType(createdFromPin.mTypeID);
		const Node& createdNode = nodeGraph.mNodes.at(createdNodeID);

		const std::vector<PinID>& pinIDs = SelectByFlowType(pinType.mFlowType, createdNode.mOutputPins, createdNode.mInputPins);

		for (const PinID pinID : pinIDs)
		{
			if (Internal::TryCreateLink(nodeGraph, pinID, aConnection, aCommandTracker))
			{
				break;
			}
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(createdNodeID, nodeGraph);
	}

	NodeView CreateGetterNode(ClassView aClassView, NodeGraphView aNodeGraphView, const VariableView aVariableView, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Getter Node");
		}

		const Variable& variable = aClassView.GetClass().GetVariableManager().mVariables.at(aVariableView.GetID());

		const NodeID nodeID = Internal::CreateGetterNode(aNodeGraphView.GetNodeGraph(), variable.dataTypeID, aCommandTracker);
		Internal::SetNodePosition(nodeID, aPosition, aNodeGraphView.GetNodeGraph(), aCommandTracker);
		Internal::BindVariable(aClassView.GetClass(), NodeRef{ .mNodeID = nodeID, .mNodeGraph = &aNodeGraphView.GetNodeGraph() }, aVariableView.GetID(), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(nodeID, aNodeGraphView.GetNodeGraph());
	}

	NodeView CreateSetterNode(ClassView aClassView, NodeGraphView aNodeGraphView, const VariableView aVariableView, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Create Setter Node");
		}

		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();

		const Variable& variable = aClassView.GetClass().GetVariableManager().mVariables.at(aVariableView.GetID());

		const NodeID nodeID = Internal::CreateSetterNode(nodeGraph, variable.dataTypeID, aCommandTracker);
		Internal::SetNodePosition(nodeID, aPosition, nodeGraph, aCommandTracker);
		Internal::BindVariable(aClassView.GetClass(), NodeRef{ .mNodeID = nodeID, .mNodeGraph = &nodeGraph }, aVariableView.GetID(), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}

		return NodeView(nodeID, nodeGraph);
	}

	LinkView TryCreateLink(const PinView aPinView1, const PinView aPinView2, NodeGraphView aNodeGraphView, CommandTracker* const aCommandTracker)
	{
		const LinkID createdLinkID = Internal::TryCreateLink(aNodeGraphView.GetNodeGraph(), aPinView1.GetID(), aPinView2.GetID(), aCommandTracker);
		return LinkView(createdLinkID, aNodeGraphView.GetNodeGraph());
	}

	void DestroyLink(const LinkView aLinkView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		Internal::DestroyLink(aNodeGraphView.GetNodeGraph(), aLinkView.GetID(), aCommandTracker);
	}

	/*void DestoryLinksByOutputPin(const PinView aOutputPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		const Pin& outputPin = aNodeGraphView.GetNodeGraph().mPins.at(aOutputPinView.GetID());
		outputPin;
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destory Links by Output Pin");
		}
		for (const PinID connectedInputPin : outputPin.mConnectedPinIDs)
		{
			DestroyLink(LinkView{ connectedInputPin, aOutputPinView.GetID() }, aNodeGraphView.GetNodeGraph(), aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}*/

	void DestroyNode(const NodeView aNodeView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		return Internal::DestroyNode(aNodeGraphView.GetNodeGraph(), aNodeView.GetID(), aCommandTracker);
	}

	void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Selection");
		}

		for (const LinkID linkID : aLinkIDs)
		{
			DestroyLink(LinkView(linkID, aNodeGraphView.GetNodeGraph()), aNodeGraphView, aCommandTracker);
		}

		for (const NodeID nodeID : aNodeIDs)
		{
			DestroyNode(NodeView(nodeID, aNodeGraphView.GetNodeGraph()), aNodeGraphView, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker)
	{
		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Destroy Nodes");
		}

		for (const NodeRef& nodeRef : aNodeRefs)
		{
			Internal::DestroyNode(*nodeRef.mNodeGraph, nodeRef.mNodeID, aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void SetNodePosition(const NodeView aNodeView, Vec2 aPosition, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		const Vec2 oldPos = Internal::GetNode(aNodeView.GetID(), aNodeGraphView.GetNodeGraph()).mPosition;
		SetNodePosition(aNodeView, aPosition, oldPos, aNodeGraphView, aCommandTracker);
	}

	void SetNodePosition(const NodeView aNodeView, Vec2 aPosition, Vec2 aOldPosition, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		Internal::SetNodePosition(aNodeView.GetID(), aPosition, aOldPosition, aNodeGraphView.GetNodeGraph(), aCommandTracker);
	}

	void CommitNodeDrag(const std::unordered_map<NodeID, NodeDragData>& aDragData, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
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
			Internal::SetNodePosition(mNodeID, dragData.endPos, dragData.startPos, aNodeGraphView.GetNodeGraph(), aCommandTracker);
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	VariableView CreateVariable(const ClassView aClassView, const DataTypeView aDataTypeView, CommandTracker* aCommandTracker)
	{
		const VarID varID = Internal::CreateVariable(aClassView.GetClass(), aDataTypeView.GetID(), aCommandTracker);
		return VariableView(varID, aClassView.GetClass());
	}

	void DestroyVariable(const VariableView aVariableView, const ClassView aClassView, CommandTracker* aCommandTracker)
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

		data.mVarID = aVariableView.GetID();
		data.mClass = &aClassView.GetClass();

		auto commandFunction = [data](eCommandType aCommandType) -> void
			{
				bool mIsDestroyed = aCommandType == eCommandType::Do;
				ScriptProxy::GetVariableRef(*data.mClass, data.mVarID).mIsDestroyed = mIsDestroyed;
			};

		if (!aCommandTracker)
		{
			commandFunction(eCommandType::Do);
		}
		else
		{
			aCommandTracker->DoCommand(Command(commandFunction, "Destroy Variable"));
		}

		const VariableManager& variableManager = ScriptProxy::GetVariableManager(aClassView.GetClass());
		DestroyNodes(variableManager.GetNodeRefsByVarID(aVariableView.GetID()), aCommandTracker);

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	void EditVariableDefaultValue(const VariableView aVariableView, const ClassView aClassView, CommandTracker*)
	{
		const Variable& variable = ScriptProxy::GetVariable(aClassView.GetClass(), aVariableView.GetID());

		if (Global::GetDataTypeManager().EditData(variable.dataTypeID, variable.defaultValueDataPtr))
		{

		}
	}

	void SetVariableDataType(const VariableView aVariableView, const DataTypeView aDataTypeView, const ClassView aClassView, CommandTracker* const aCommandTracker)
	{
		Variable& variable = ScriptProxy::GetVariableRef(aClassView.GetClass(), aVariableView.GetID());

		void* defaultValueDataPtr = Global::GetDataTypeManager().AllocateData(aDataTypeView.GetID(), ScriptProxy::GetVariableMemoryArena(aClassView.GetClass()));

		variable.dataTypeID = aDataTypeView.GetID();
		variable.defaultValueDataPtr = defaultValueDataPtr;

		DestroyVariableNodes(aVariableView, aClassView, aCommandTracker);
	}

	void SetVariableName(const VariableView aVariableView, const std::string_view aName, const ClassView aClassView)
	{
		Variable& variable = ScriptProxy::GetVariableRef(aClassView.GetClass(), aVariableView.GetID());

		variable.mName = aName;
	}

	void DestroyVariableNodes(const VariableView aVariableView, ClassView aClassView, CommandTracker* const aCommandTracker)
	{
		DestroyNodes(aClassView.GetClass().GetVariableManager().GetNodeRefsByVarID(aVariableView.GetID()), aCommandTracker);
	}

	void EditPin(const PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();
		Pin& pin = nodeGraph.mPins.at(aPinView.GetID());
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

		data.mPinID = aPinView.GetID();
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

	void SplitPin(const PinView aPinView, NodeGraphView aNodeGraphView, CommandTracker* aCommandTracker)
	{
		aCommandTracker;
		NodeGraph& nodeGraph = aNodeGraphView.GetNodeGraph();
		const Pin& pin = nodeGraph.mPins.at(aPinView.GetID());

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

		const std::vector<Property>& mProperties = pinDataType->mProperties;
		if (mProperties.empty())
		{
			return;
		}

		for (const Property& property : mProperties)
		{
			const PinTypeID createdPinTypeID = Global::GetPinTypeManager().Create(property.mName, pinType.mFlowType, property.mTypeID, CreatePinSetFunction());
			Internal::CreatePin(nodeGraph, pin.mNodeID, createdPinTypeID);
		}
	}

	bool IsPinReplacable(const PinView aPinView, const NodeGraphView aNodeGraphView)
	{
		const Pin& pin = Internal::GetPin(aPinView.GetID(), aNodeGraphView.GetNodeGraph());
		const Node& node = Internal::GetNode(pin.mNodeID, aNodeGraphView.GetNodeGraph());
		const NodeType& nodeType = Internal::GetNodeType(node);

		return nodeType.mNodeRecipe.mOperatorTrait != eNodeOperatorTrait::None;
	}

	void ReplaceWildcardNode(const PinView aPinView, NodeGraphView aNodeGraphView, const DataTypeView aDataTypeView, CommandTracker* const aCommandTracker)
	{
		Internal::ReplaceWildcardNode(aNodeGraphView.GetNodeGraph(), aPinView.GetID(), aDataTypeView.GetID(), aCommandTracker);
	}

	std::vector<DataTypeView> GetReplacableDataTypes(const PinView aPinView, const NodeGraphView aNodeGraphView)
	{
		const Node& node = Internal::GetNode(aPinView.GetNodeID(), aNodeGraphView.GetNodeGraph());
		const NodeType& nodeType = Internal::GetNodeType(node);
		std::vector<DataTypeView> dataTypeViews;

		auto& dataTypes = Global::GetNodeTypeManager().GetWildcardMapByOperator(nodeType.mNodeRecipe.mOperatorTrait);
		dataTypeViews.reserve(dataTypes.size());
		for (auto& [dataTypeID, nodeTypeID] : dataTypes)
		{
			dataTypeViews.push_back(DataTypeView(dataTypeID));
		}

		return dataTypeViews;
	}

	void SetPinTypeName(const PinTypeView aPinTypeView, std::string_view aName)
	{
		PinType& pinType = Global::GetPinTypeManager().GetPinType(aPinTypeView.GetID());

		pinType.mName = aName;
	}

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraphView aCopiedFromNodeGraphView)
	{
		if (aNodeIDs.empty())
		{
			return;
		}
		const NodeGraph& copiedFromNodeGraph = aCopiedFromNodeGraphView.GetNodeGraph();
		CopyBuffer& copyBuffer = Global::Internal::GetCopyBuffer();
		copyBuffer = CopyBuffer{};

		std::vector<NodeID> createdNodeIDs;
		std::unordered_map<NodeID, NodeID> nodeConverter;
		createdNodeIDs.reserve(aNodeIDs.size());

		Vec2 avgPos;
		for (const NodeID nodeID : aNodeIDs)
		{
			const Node& node = copiedFromNodeGraph.mNodes.at(nodeID);

			const NodeID createdNodeID = Internal::CreateNode(&copyBuffer.mNodeGraph, node.mTypeID, node.mPosition, nullptr);
			avgPos += node.mPosition;

			createdNodeIDs.push_back(createdNodeID);
			nodeConverter.emplace(nodeID, createdNodeID);
		}

		avgPos /= static_cast<float>(aNodeIDs.size());

		for (Node& node : copyBuffer.mNodeGraph.mNodes)
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
				const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(copiedFromNodeGraph, copiedInputPinID, copyBuffer.mNodeGraph, createdNodeID);
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
					const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(copiedFromNodeGraph, connectedOutputPinID, copyBuffer.mNodeGraph, newConnectedNodeID);

					Internal::TryCreateLink(copyBuffer.mNodeGraph, createdInputPinID, createdOutputPinID, nullptr);
				}

				Pin& createdInputPin = Internal::GetPin(createdInputPinID, copyBuffer.mNodeGraph);

				Global::GetDataTypeManager().CopyData(copiedInputPinType.mDataTypeID, createdInputPin.mDataPtr, copiedInputPin.mDataPtr);
			}
		}

		// Copy Data

	}

	void PasteCopyBuffer(const Vec2 aPosition, NodeGraphView aTargetNodeGraphView, CommandTracker* const aCommandTracker)
	{
		CopyBuffer& copyBuffer = Global::Internal::GetCopyBuffer();

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Paste Nodes");
		}

		NodeGraph& targetNodeGraph = aTargetNodeGraphView.GetNodeGraph();

		std::unordered_map<NodeID, NodeID> nodeConverter;

		for (NodeID sourceNodeID = 0; sourceNodeID < copyBuffer.mNodeGraph.mNodes.size(); sourceNodeID++)
		{
			const Node& node = copyBuffer.mNodeGraph.mNodes[sourceNodeID];
			const NodeID createdNodeID = Internal::CreateNode(&targetNodeGraph, node.mTypeID, aPosition + node.mPosition, aCommandTracker);
			nodeConverter.emplace(sourceNodeID, createdNodeID);

			const Node& createdNode = Internal::GetNode(createdNodeID, targetNodeGraph);
			DataTypeManager& dataTypeManager = Global::GetDataTypeManager();
			for (const PinID createdInputPinID : createdNode.mInputPins)
			{
				Pin& createdInputPin = Internal::GetPin(createdInputPinID, targetNodeGraph);
				const PinType& createdInputPinType = Global::GetPinTypeManager().GetPinType(createdInputPin.mTypeID);

				const PinID sourcePinID = ScriptLinker::GetOpposingPinID(targetNodeGraph, createdInputPinID, copyBuffer.mNodeGraph, sourceNodeID);
				const Pin& sourcePin = ScriptProxy::GetPin(copyBuffer.mNodeGraph, sourcePinID);
				dataTypeManager.CopyData(createdInputPinType.mDataTypeID, createdInputPin.mDataPtr, sourcePin.mDataPtr);
			}
		}

		for (const Link& link : copyBuffer.mNodeGraph.mLinks)
		{
			const Pin& inputPin = copyBuffer.mNodeGraph.mPins[link.mInputPinID];
			const Pin& outputPin = copyBuffer.mNodeGraph.mPins[link.mOutputPinID];
			const PinID createdInputPinID = ScriptLinker::GetOpposingPinID(copyBuffer.mNodeGraph, link.mInputPinID, targetNodeGraph, nodeConverter.at(inputPin.mNodeID));
			const PinID createdOutputPinID = ScriptLinker::GetOpposingPinID(copyBuffer.mNodeGraph, link.mOutputPinID, targetNodeGraph, nodeConverter.at(outputPin.mNodeID));
			const LinkID createdLinkID = Internal::TryCreateLink(targetNodeGraph, createdInputPinID, createdOutputPinID, aCommandTracker);

			assert(createdLinkID != InvalidID<LinkID>());
		}

		if (aCommandTracker)
		{
			aCommandTracker->EndComposite();
		}
	}

	CustomEventView CreateCustomEvent(const std::string_view aName)
	{
		return CustomEventView(Internal::CreateCustomEvent(aName));
	}

	static PinTypeID AddPinToNodeType(const DataTypeID aDataTypeID, const NodeTypeID aNodeTypeID, const eFlowType aFlowType, std::string_view aPinName)
	{
		NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);

		const PinTypeID createdPinTypeID = Global::GetPinTypeManager().Create(aPinName, aFlowType, aDataTypeID, CreatePinSetFunction());

		std::vector<PinTypeID>& pinTypeIDs = aFlowType == eFlowType::Input ? nodeType.mNodeRecipe.mInputPinTypeIDs : nodeType.mNodeRecipe.mOutputPinTypeIDs;
		pinTypeIDs.push_back(createdPinTypeID);

		for (const NodeRef& nodeRef : nodeType.mNodeRefs)
		{
			Node& node = ScriptProxy::GetNodeRef(*nodeRef.mNodeGraph, nodeRef.mNodeID);

			std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
			const PinID createdPinID = Internal::CreatePin(*nodeRef.mNodeGraph, nodeRef.mNodeID, createdPinTypeID);

			pinIDs.push_back(createdPinID);
		}

		return createdPinTypeID;
	}


	static void SetPinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t anIndex, const DataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		PinTypeManager& pinTypeManager = Global::GetPinTypeManager();

		NodeType& nodeType = nodeTypeManager.GetNodeType(aNodeTypeID);
		std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);
		const PinTypeID oldPinTypeID = pinTypeIDs.at(anIndex);
		const PinType& oldPinType = pinTypeManager.GetPinType(oldPinTypeID);

		const PinTypeID newPinTypeID = pinTypeManager.Create(oldPinType.mName, aFlowType, aDataTypeID, CreatePinSetFunction());

		pinTypeIDs.at(anIndex) = newPinTypeID;

		const std::vector<NodeRef>& mNodeRefs = nodeType.mNodeRefs;
		for (const NodeRef& nodeRef : mNodeRefs)
		{
			Node& node = ScriptProxy::GetNodeRef(*nodeRef.mNodeGraph, nodeRef.mNodeID);

			std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
			const PinID createdPinID = Internal::CreatePin(*nodeRef.mNodeGraph, nodeRef.mNodeID, newPinTypeID);
			pinIDs.at(anIndex) = createdPinID;
		}
	}

	static void DeletePinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t anIndex, const eFlowType aFlowType)
	{
		NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);

		std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);

		assert(anIndex < pinTypeIDs.size());

		pinTypeIDs.erase(pinTypeIDs.begin() + anIndex);

		for (const NodeRef& nodeRef : nodeType.mNodeRefs)
		{
			Node& node = ScriptProxy::GetNodeRef(*nodeRef.mNodeGraph, nodeRef.mNodeID);

			std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
			pinIDs.erase(pinIDs.begin() + anIndex);
		}
	}

	void AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aCustomEventID, std::string_view aPinName)
	{
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

		AddPinToNodeType(aDataTypeID, customEvent.GetExecutorTypeID(), eFlowType::Output, aPinName);
		AddPinToNodeType(aDataTypeID, customEvent.GetCallerTypeID(), eFlowType::Input, aPinName);
	}

	void SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeView aDataTypeView, const CustomEventID aCustomEventID)
	{
		if (anIndex == 0)
		{
			return;
		}
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

		SetPinAtIndexNodeType(customEvent.GetExecutorTypeID(), anIndex, aDataTypeView.GetID(), eFlowType::Output);
		SetPinAtIndexNodeType(customEvent.GetCallerTypeID(), anIndex, aDataTypeView.GetID(), eFlowType::Input);
	}

	void DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aCustomEventID)
	{
		if (anIndex == 0)
		{
			return;
		}

		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

		DeletePinAtIndexNodeType(customEvent.GetCallerTypeID(), anIndex, eFlowType::Input);
		DeletePinAtIndexNodeType(customEvent.GetExecutorTypeID(), anIndex, eFlowType::Output);
	}

	void SetCustomEventName(const CustomEventID aCustomEventID, std::string_view aName)
	{
		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);
		NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		NodeType& callerNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetCallerTypeID());

		const std::string nameDirectory = Global::GetNodeTypeManager().GetNameDirectory(customEvent.GetExecutorTypeID());
		executorNodeType.mName = nameDirectory + std::string(aName);
		callerNodeType.mName = nameDirectory + "Call " + std::string(aName);
	}

	FunctionView CreateGlobalFunction(const std::string_view aName)
	{
		return FunctionView(Internal::CreateFunction(aName));
	}

	FunctionView CreateMemberFunction(const std::string_view aName, ClassView aClassView)
	{
		const FunctionID id = Internal::CreateFunction(aName);
		aClassView.GetClass().BindFunction(id);

		return FunctionView(id);
	}

	void AddPinToFunction(const FunctionView aFunctionView, const DataTypeID aDataTypeID, const eFlowType aFlowType, std::string_view aPinName)
	{
		const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
		const Function& function = nodeTypeManager.GetFunction(aFunctionView.GetID());


		AddPinToNodeType(aDataTypeID, function.mCallerNodeTypeID, aFlowType, aPinName);

		const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
		AddPinToNodeType(aDataTypeID, inputOutputNodeTypeID, InvertFlowType(aFlowType), aPinName);
	}

	void SetPinAtIndexFunction(const FunctionView aFunctionView, const size_t anIndex, const DataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		const Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionView.GetID());

		SetPinAtIndexNodeType(function.mCallerNodeTypeID, anIndex, aDataTypeID, aFlowType);

		const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
		SetPinAtIndexNodeType(inputOutputNodeTypeID, anIndex, aDataTypeID, InvertFlowType(aFlowType));
	}

	void DeletePinAtIndexFunction(const FunctionView aFunctionView, const size_t anIndex, const eFlowType aFlowType)
	{
		const Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionView.GetID());

		DeletePinAtIndexNodeType(function.mCallerNodeTypeID, anIndex, aFlowType);

		const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
		DeletePinAtIndexNodeType(inputOutputNodeTypeID, anIndex, InvertFlowType(aFlowType));
	}

	void SetFunctionName(const FunctionView aFunctionView, std::string_view aName)
	{
		Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionView.GetID());

		function.mName = aName;
	}

	void BeginFrame()
	{
		Global::Internal::GetFrameMemoryArena().Clear();
	}

	VariableView GetVariableByNode(const NodeView aNodeView, NodeGraphView aNodeGraphView, const ClassView aClassView)
	{
		const VariableManager& variableManager = ScriptProxy::GetVariableManager(aClassView.GetClass());
		const VarID varID = variableManager.GetVariableIDByNodeRef(NodeRef{ .mNodeID = aNodeView.GetID(), .mNodeGraph = &aNodeGraphView.GetNodeGraph() });
		return VariableView(varID, aClassView.GetClass());
	}

	std::vector<VariableView> GetVariables(const ClassView aClass, const bool aIncludeDestroyed)
	{
		const std::vector<Variable>& variables = aClass.GetClass().GetVariableManager().mVariables;
		std::vector<VariableView> views;

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
			views.push_back(VariableView(varID, aClass.GetClass()));
		}
		return views;
	}

	std::vector<NodeView> GetNodes(const NodeGraphView aNodeGraphView, bool aIncludeDestroyed)
	{
		const std::vector<Node>& nodes = aNodeGraphView.GetNodeGraph().mNodes;
		std::vector<NodeView> views;

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
			views.push_back(NodeView(mNodeID, aNodeGraphView.GetNodeGraph()));
		}
		return views;
	}

	std::vector<LinkView> GetLinks(const NodeGraphView aNodeGraphView, bool aIncludeDestroyed)
	{
		const std::vector<Link>& links = aNodeGraphView.GetNodeGraph().mLinks;
		std::vector<LinkView> views;

		views.reserve(links.size());

		for (LinkID linkID = 0; linkID < links.size(); ++linkID)
		{
			if (!aIncludeDestroyed)
			{
				const Link& link = aNodeGraphView.GetNodeGraph().mLinks.at(linkID);
				if (link.mIsDestroyed)
				{
					continue;
				}
			}
			views.push_back(LinkView(linkID, aNodeGraphView.GetNodeGraph()));
		}
		return views;
	}

	std::vector<DataTypeView> GetDataTypes()
	{
		const auto& dataTypes = Global::GetDataTypeManager().GetDataTypes();
		std::vector<DataTypeView> views;
		views.reserve(dataTypes.size());

		for (const auto& [dataTypeID, dataType] : dataTypes)
		{
			views.push_back(DataTypeView(dataTypeID));
		}

		return views;
	}

	std::vector<NodeTypeView> GetNodeTypes()
	{
		const auto& nodeTypes = Global::GetNodeTypeManager().GetNodeTypes();
		std::vector<NodeTypeView> views;
		views.reserve(nodeTypes.size());

		for (NodeTypeID nodeTypeID = 0; nodeTypeID < nodeTypes.size(); ++nodeTypeID)
		{
			views.push_back(NodeTypeView(nodeTypeID));
		}

		return views;
	}

	std::vector<FunctionView> GetFunctions()
	{
		const auto& mFunctions = Global::GetNodeTypeManager().GetFunctions();
		std::vector<FunctionView> views;
		views.reserve(mFunctions.size());

		for (FunctionID functionID = 0; functionID < mFunctions.size(); ++functionID)
		{
			views.push_back(FunctionView(functionID));
		}

		return views;
	}

	std::vector<CustomEventView> GetCustomEvents()
	{
		const auto& customEvents = Global::GetNodeTypeManager().GetCustomEvents();
		std::vector<CustomEventView> customEventViews;
		customEventViews.reserve(customEvents.size());

		for (CustomEventID customEventID = 0; customEventID < customEvents.size(); ++customEventID)
		{
			customEventViews.push_back(CustomEventView(customEventID));
		}

		return customEventViews;
	}

	std::vector<NodeTypeView> GetNodeTypesFilteredByDataTypeAndFlowType(const DataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		return Global::GetNodeTypeManager().GetNodeTypesFiltered([aDataTypeID, aFlowType](const NodeType& aNodeType) -> bool
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

	std::vector<NodeTypeView> GetNodeTypesFilteredByTrait(const eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
	{
		return Global::GetNodeTypeManager().GetNodeTypesFiltered([aNodeTrait, aBitOperation](const NodeType& aNodeType) -> bool
			{
				return aBitOperation(aNodeTrait, aNodeType.mNodeRecipe.mTraits);
			}
		);
	}

	std::unordered_map<DataTypeView, std::vector<ClassView>> GetClasses()
	{
		auto& classes = ScriptFoundation::GetInstance().GetClasses();

		std::unordered_map<DataTypeView, std::vector<ClassView>> views;

		for (const auto& [dataTypeID, classesByDataTypeID] : classes)
		{
			std::vector<ClassView>& classVector = views[DataTypeView(dataTypeID)];

			for (const auto& flyClass : classesByDataTypeID)
			{
				classVector.push_back(ClassView(*flyClass));
			}
		}

		return views;
	}
}

