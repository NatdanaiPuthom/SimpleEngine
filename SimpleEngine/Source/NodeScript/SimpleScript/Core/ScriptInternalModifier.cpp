#include "ScriptInternalModifier.hpp"
#include "FlyClass.hpp"
#include "CustomEvent/CustomEvent.hpp"
#include "Node/NodeTypeManager.hpp"
#include "Pin/PinTypeManager.hpp"
#include "DataType/DataTypeManager.hpp"
#include "Command/ScriptCommandTracker.hpp"
#include "Node/NodeExecutor.hpp"
#include "Utilities/ScriptLinker.hpp"
#include "ScriptModifier.hpp"
#include "Global/FlyGlobal.hpp"
#include "FlyFlow.hpp"
#include "FlyWildcard.hpp"
#include "Utilities/ScriptProxy.hpp"

namespace FLY_NAMESPACE
{

	namespace Internal
	{

		NodeGraph& GetNodeGraph(NodeGraphVariant&& aNodeGraphVariant)
		{
			switch (aNodeGraphVariant.index())
			{
			case 0:
				return *std::get<0>(aNodeGraphVariant);
				break; 
			case 1:
				return std::get<1>(aNodeGraphVariant)->mNodeGraph;
				break;
			default:
				break;
			}
			return *std::get<0>(aNodeGraphVariant);
		}

		void BindNodeToEvent(const NodeID aNodeID, EventGraph& anEventGraph, CommandTracker* const aCommandTracker)
		{
			struct BindData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				EventGraph* eventGraph;
			} data;

			data.mNodeID = aNodeID;
			data.eventGraph = &anEventGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					void (EventGraph:: * func)(NodeID) = aCommandType == eCommandType::Do ? &EventGraph::BindNodeToEvent : &EventGraph::UnbindNodeFromEvent;

					(data.eventGraph->*func)(data.mNodeID);
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Bind Node To Event"));
			}
		}


		CustomEventID CreateCustomEvent(std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateCustomEvent(aName);
		}

		FunctionID CreateFunction(std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateFunction(aName);
		}

		NodeID CreateNode(NodeGraphVariant&& aNodeGraphVariant, const NodeTypeID aNodeTypeID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Position");
			}

			NodeGraph& nodeGraph = GetNodeGraph(std::forward<NodeGraphVariant>(aNodeGraphVariant));
			const NodeID nodeID = GetCurrentNodeID(nodeGraph);
			AddNode(nodeGraph, Global::GetNodeTypeManager().CreateNode(nodeGraph, nodeID, aNodeTypeID), nodeID, aCommandTracker);

			SetNodePosition(nodeID, aPosition, nodeGraph, aCommandTracker);

			std::visit(
				[&](auto&& aType) -> void
				{
					using Type = std::decay_t<decltype(aType)>;
					if constexpr (std::same_as<Type, EventGraph*>)
					{
						BindNodeToEvent(nodeID, *aType, aCommandTracker);
					}

				}, std::forward<NodeGraphVariant>(aNodeGraphVariant)
			);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
			return nodeID;
		}


		NodeID CreateNode(NodeGraphVariant&& aNodeGraphVariant, const std::string_view aName, bool& aSuccess, const Vec2 aPosition, bool aCreateIfNameNotFound, CommandTracker* const aCommandTracker)
		{
			const NodeTypeID typeID = Global::GetNodeTypeManager().GetTypeID(aName);
			aSuccess = typeID != 0;
			if (!aCreateIfNameNotFound && !aSuccess)
			{
				return InvalidID<NodeID>();
			}
			return CreateNode(std::forward<NodeGraphVariant>(aNodeGraphVariant), typeID, aPosition, aCommandTracker);
		}

		NodeID CreateGetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, Global::GetNodeTypeManager().CreateGetterNode(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateSetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, Global::GetNodeTypeManager().CreateSetterNode(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, Global::GetNodeTypeManager().CreateOperatorNode(aNodeGraph, id, aOperatorTrait, aDataTypeID), id, aCommandTracker);
			return id;
		}

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* aCommandTracker)
		{
			std::vector<Node>& nodes = ScriptProxy::GetNodes(aNodeGraph);

			nodes.emplace_back(std::move(aNode));

			Node& createdNode = nodes.back();

			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(createdNode.mTypeID);
			nodeType.mNodeRefs.push_back(NodeRef{ .mNodeID = aNodeID, .mNodeGraph = &aNodeGraph });

			struct CreateNodeData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mNodeID = aNodeID;
			data.mNodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					Node& node = ScriptProxy::GetNodeRef(*data.mNodeGraph, data.mNodeID);

					node.mIsDestroyed = aCommandType == eCommandType::Undo;
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Create Node"));
			}
		}

		void DestroyNode(NodeGraph& aNodeGraph, const NodeID aNodeID, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Node + Conncected Links");

			}

			struct DestroyNodeData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mNodeID = aNodeID;
			data.mNodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					Node& node = data.mNodeGraph->mNodes[data.mNodeID];
					node.mIsDestroyed = aCommandType == eCommandType::Do;
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Destroy Node"));
			}

			for (const LinkID linkID : ScriptLinker::GetLinkIDsByNode(aNodeGraph, aNodeID))
			{
				DestroyLink(aNodeGraph, linkID, aCommandTracker);
			}


			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}
		void SetNodePosition(const NodeID aNodeID, const Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			const Vec2 oldPos = aNodeGraph.mNodes.at(aNodeID).mPosition;
			SetNodePosition(aNodeID, aPosition, oldPos, aNodeGraph, aCommandTracker);
		}
		void SetNodePosition(const NodeID aNodeID, const Vec2 aPosition, const Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{

			struct SetNodePositionData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				Vec2 oldPos;
				Vec2 newPos;
				NodeGraph* mNodeGraph = nullptr;
			} data;

			if (aPosition == aOldPosition)
			{
				return;
			}


			data.mNodeID = aNodeID;
			data.oldPos = aOldPosition;
			data.newPos = aPosition;
			data.mNodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					Node& node = data.mNodeGraph->mNodes.at(data.mNodeID);
					const Vec2& pos = aCommandType == eCommandType::Do ? data.newPos : data.oldPos;
					node.mPosition = pos;
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Set Node Position"));
			}


		}

		std::vector<PinID> CreateInputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, size_t aStartIndex)
		{
			const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);
			const std::vector<PinTypeID>& pinTypeIDs = nodeType.mNodeRecipe.mInputPinTypeIDs;
			std::vector<PinID> pinsIDs;

			for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
			{
				pinsIDs.push_back(CreatePin(aNodeGraph, aNodeID, pinTypeIDs[i]));
			}

			return pinsIDs;
		}

		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID)
		{
			const DataTypeID dataTypeID = Global::GetPinTypeManager().GetPinType(aPinTypeID).mDataTypeID;

			MemoryArena<NodeBufferCapacity>& memoryArena = ScriptProxy::GetNodeGraphMemoryArena(aNodeGraph);

			void* const mDataPtr = Global::GetDataTypeManager().AllocateData(dataTypeID, memoryArena);

			return CreatePin(aNodeGraph, aNodeID, aPinTypeID, mDataPtr);
		}


		std::vector<PinID> CreateOutputPins(NodeGraph& aNodeGraph, const NodeID aNodeID, const NodeTypeID aNodeTypeID, const size_t aStartIndex)
		{
			const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);
			const std::vector<PinTypeID>& pinTypeIDs = nodeType.mNodeRecipe.mOutputPinTypeIDs;
			std::vector<PinID> pinsIDs;

			for (size_t i = aStartIndex; i < pinTypeIDs.size(); i++)
			{
				pinsIDs.push_back(CreatePin(aNodeGraph, aNodeID, pinTypeIDs[i]));
			}

			return pinsIDs;
		}


		PinID CreatePin(NodeGraph& aNodeGraph, const NodeID aNodeID, const PinTypeID aPinTypeID, void* const aDataPtr)
		{
			std::vector<Pin>& pins = ScriptProxy::GetPins(aNodeGraph);
			const PinID id = static_cast<PinID>(pins.size());
			pins.push_back(Pin{ aNodeID, aPinTypeID, aDataPtr });
			return id;
		}

		void ActivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.mLinks[aLinkID];


			{
				Pin& inputPin = ScriptProxy::GetPinRef(aNodeGraph, link.mInputPinID);
				auto it = std::find(inputPin.mConnectedPinIDs.begin(), inputPin.mConnectedPinIDs.end(), link.mOutputPinID);
				if (it == inputPin.mConnectedPinIDs.end())
				{
					inputPin.mConnectedPinIDs.push_back(link.mOutputPinID);
				}
			}
			{
				Pin& outputPin = ScriptProxy::GetPinRef(aNodeGraph, link.mOutputPinID);
				auto it = std::find(outputPin.mConnectedPinIDs.begin(), outputPin.mConnectedPinIDs.end(), link.mInputPinID);
				if (it == outputPin.mConnectedPinIDs.end())
				{
					outputPin.mConnectedPinIDs.push_back(link.mInputPinID);
				}
			}

			link.mIsDestroyed = false;
		}

		void DeactivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.mLinks[aLinkID];

			Pin& inputPin = ScriptProxy::GetPinRef(aNodeGraph, link.mInputPinID);
			Pin& outputPin = ScriptProxy::GetPinRef(aNodeGraph, link.mOutputPinID);

			std::erase(inputPin.mConnectedPinIDs, link.mOutputPinID);
			std::erase(outputPin.mConnectedPinIDs, link.mInputPinID);

			link.mIsDestroyed = true;
		}

		LinkID TryCreateLink(NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2, CommandTracker* aCommandTracker)
		{
			const Link createdLink = ScriptLinker::ArePinsLinkable(aNodeGraph, aPinID1, aPinID2);
			if (!createdLink)
			{
				// Check if we can replace node with overloaded operator node
				const Pin& pin1 = aNodeGraph.mPins.at(aPinID1);
				const Pin& pin2 = aNodeGraph.mPins.at(aPinID2);

				const PinType& pinType1 = Global::GetPinTypeManager().GetPinType(pin1.mTypeID);
				const PinType& pinType2 = Global::GetPinTypeManager().GetPinType(pin2.mTypeID);

				if (pinType1.mDataTypeID == GetDataTypeID<Wildcard>())
				{
					ReplaceWildcardNode(aNodeGraph, aPinID1, aPinID2, aCommandTracker);

				}
				else if (pinType2.mDataTypeID == GetDataTypeID<Wildcard>())
				{
					ReplaceWildcardNode(aNodeGraph, aPinID2, aPinID1, aCommandTracker);
				}

				return InvalidID<LinkID>();
			}

			return CreateLink(aNodeGraph, createdLink.mInputPinID, createdLink.mOutputPinID, aCommandTracker);
		}

		LinkID CreateLink(NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aOutputPinID, CommandTracker* aCommandTracker)
		{
			assert(aInputPinID != InvalidID<PinID>());
			assert(aOutputPinID != InvalidID<PinID>());

			struct CreateLinkData
			{
				LinkID createdLinkID = InvalidID<LinkID>();
				LinkID previousLinkID = InvalidID<LinkID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mNodeGraph = &aNodeGraph;
			data.createdLinkID = static_cast<LinkID>(aNodeGraph.mLinks.size());


			const Pin& inputPin = ScriptProxy::GetPin(aNodeGraph, aInputPinID);
			const Pin& outputPin = ScriptProxy::GetPin(aNodeGraph, aOutputPinID);
			const PinType& inputPinType = Global::GetPinTypeManager().GetPinType(inputPin.mTypeID);
			const PinType& outputPinType = Global::GetPinTypeManager().GetPinType(outputPin.mTypeID);
			DataTypeID inputPinDataType = inputPinType.mDataTypeID;
			DataTypeID outputPinDataType = outputPinType.mDataTypeID;
			assert(inputPinType.mFlowType == eFlowType::Input);
			assert(outputPinType.mFlowType == eFlowType::Output);
			assert(inputPinDataType == outputPinDataType);

			if (inputPinDataType != GetDataTypeID<Flow>())
			{
				std::vector<LinkID> inputLinkIDs = ScriptLinker::GetLinkIDsByPin(aNodeGraph, aInputPinID);
				if (!inputLinkIDs.empty())
				{
					assert(inputLinkIDs.size() == 1);
					data.previousLinkID = inputLinkIDs.front();
				}
			}
			else
			{
				std::vector<LinkID> outputLinkIDs = ScriptLinker::GetLinkIDsByPin(aNodeGraph, aOutputPinID);

				if (!outputLinkIDs.empty())
				{
					assert(outputLinkIDs.size() == 1);
					data.previousLinkID = outputLinkIDs.front();
				}

			}

			aNodeGraph.mLinks.push_back(Link{ aInputPinID, aOutputPinID });

			auto doAction = [](const CreateLinkData& aData) -> void
				{
					if (aData.previousLinkID != InvalidID<LinkID>())
					{
						DeactivateLink(*aData.mNodeGraph, aData.previousLinkID);
					}

					ActivateLink(*aData.mNodeGraph, aData.createdLinkID);
				};

			auto commandFunction = [data](eCommandType aCommandType)
				{
					if (aCommandType == eCommandType::Do)
					{

						if (data.previousLinkID != InvalidID<LinkID>())
						{
							DeactivateLink(*data.mNodeGraph, data.previousLinkID);
						}

						ActivateLink(*data.mNodeGraph, data.createdLinkID);
					}
					else
					{
						DeactivateLink(*data.mNodeGraph, data.createdLinkID);
						if (data.previousLinkID != InvalidID<LinkID>())
						{
							ActivateLink(*data.mNodeGraph, data.previousLinkID);
						}
					}
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Create Link"));
			}

			return data.createdLinkID;
		}


		void DestroyLink(NodeGraph& aNodeGraph, const LinkID aLinkID, CommandTracker* aCommandTracker)
		{
			assert(aLinkID != InvalidID<LinkID>());

			struct DestroyLinkData
			{
				LinkID destroyedLinkID = InvalidID<LinkID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.destroyedLinkID = aLinkID;
			data.mNodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					void (*func) (NodeGraph&, LinkID) = aCommandType == eCommandType::Do ? DeactivateLink : ActivateLink;

					func(*data.mNodeGraph, data.destroyedLinkID);
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Destory Link"));
			}
		}

		VarID CreateVariable(Class& aClass, const DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			std::vector<Variable>& variables = ScriptProxy::GetVariablesRef(aClass);
			const VarID id = variables.size();
			variables.emplace_back();
			SetVariableDataType(VariableView(id, aClass), DataTypeView(aDataTypeID), ClassView(aClass), aCommandTracker);
			return id;
		}

		void BindVariable(Class& aClass, const NodeRef& aNodeRef, const VarID aVarID, CommandTracker* aCommandTracker)
		{
			struct BindVarData
			{
				NodeRef mNodeRef;
				VarID mVarID = InvalidID<VarID>();
				Class* mClass = nullptr;
			} data;

			data.mNodeRef = aNodeRef;
			data.mVarID = aVarID;
			data.mClass = &aClass;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.mClass)[data.mNodeRef] = data.mVarID;
					}
					else
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.mClass).erase(data.mNodeRef);
					}
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Bind Node To Variable"));
			}
		}

		void UnbindVariable(Class& aClass, const NodeRef& aNodeRef, CommandTracker* aCommandTracker)
		{
			if (!ScriptProxy::GetNodeRefToVarIDMap(aClass).contains(aNodeRef))
			{
				return;
			}

			struct UnbindVarData
			{
				NodeRef nodeRef;
				VarID varID = InvalidID<VarID>();
				Class* script = nullptr;
			} data;

			data.nodeRef = aNodeRef;
			data.varID = ScriptProxy::GetNodeRefToVarIDMap(aClass).at(data.nodeRef);
			data.script = &aClass;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script).erase(data.nodeRef);
					}
					else
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script)[data.nodeRef] = data.varID;
					}
				};

			if (!aCommandTracker)
			{
				commandFunction(eCommandType::Do);
			}
			else
			{
				aCommandTracker->DoCommand(Command(commandFunction, "Unbind Variable"));
			}
		}

		void ReplaceWildcardNode(NodeGraph& aNodeGraph, const PinID aWildcardPinID, const PinID aConnectedPinID, CommandTracker* const aCommandTracker)
		{
			const Pin& wildcardPin = aNodeGraph.mPins.at(aWildcardPinID);
			const Pin& connectedPin = aNodeGraph.mPins.at(aConnectedPinID);

			const NodeID wildcardNodeID = wildcardPin.mNodeID;

			const NodeType& wildcardNodeType = Global::GetNodeTypeManager().GetNodeType(aNodeGraph.mNodes.at(wildcardNodeID).mTypeID);

			if (wildcardNodeType.mNodeRecipe.mOperatorTrait == eNodeOperatorTrait::None)
			{
				return;
			}

			const PinType& wildcardPinType = Global::GetPinTypeManager().GetPinType(wildcardPin.mTypeID);
			const PinType& connectedPinType = Global::GetPinTypeManager().GetPinType(connectedPin.mTypeID);
			const bool canReplace = Global::GetNodeTypeManager().CanCreateOperatorNode(wildcardNodeType.mNodeRecipe.mOperatorTrait, connectedPinType.mDataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, wildcardNodeType.mNodeRecipe.mOperatorTrait, connectedPinType.mDataTypeID, aCommandTracker);



			DestroyNode(aNodeGraph, wildcardNodeID, aCommandTracker);


			Node& createdNode = aNodeGraph.mNodes.at(createdNodeID);
			Node& replacedNode = aNodeGraph.mNodes.at(wildcardNodeID);
			SetNodePosition(createdNodeID, replacedNode.mPosition, aNodeGraph, nullptr);

			{ // Link new pin
				const size_t pinIndex = ScriptLinker::GetPinIndex(aNodeGraph, aWildcardPinID);

				const PinID createdPinConnectedID = SelectByFlowType(wildcardPinType.mFlowType, createdNode.mInputPins.at(pinIndex), createdNode.mOutputPins.at(pinIndex));

				TryCreateLink(aNodeGraph, aConnectedPinID, createdPinConnectedID, aCommandTracker);
			}

			{ // Link previously linked pins


				for (size_t pinIndex = 0; pinIndex < replacedNode.mInputPins.size(); ++pinIndex)
				{
					const Pin& destroyedInputPin = aNodeGraph.mPins.at(replacedNode.mInputPins.at(pinIndex));

					if (!destroyedInputPin.mConnectedPinIDs.empty())
					{
						TryCreateLink(aNodeGraph, destroyedInputPin.mConnectedPinIDs[0], ScriptLinker::GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Input), aCommandTracker);
					}

				}

				for (size_t pinIndex = 0; pinIndex < replacedNode.mOutputPins.size(); ++pinIndex)
				{
					const Pin& destroyedOutputPin = aNodeGraph.mPins.at(replacedNode.mOutputPins.at(pinIndex));

					for (const PinID connectedInputPinID : destroyedOutputPin.mConnectedPinIDs)
					{
						if (connectedInputPinID != InvalidID<PinID>())
						{
							TryCreateLink(aNodeGraph, connectedInputPinID, ScriptLinker::GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Output), aCommandTracker);
						}
					}
				}
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

		}

		void ReplaceWildcardNode(NodeGraph& aNodeGraph, const PinID aWildcardPinID, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const Pin& wildcardPin = aNodeGraph.mPins.at(aWildcardPinID);

			const NodeID wildcardNodeID = wildcardPin.mNodeID;

			const NodeType& wildcardNodeType = Global::GetNodeTypeManager().GetNodeType(aNodeGraph.mNodes.at(wildcardNodeID).mTypeID);

			if (wildcardNodeType.mNodeRecipe.mOperatorTrait == eNodeOperatorTrait::None)
			{
				return;
			}

			const bool canReplace = Global::GetNodeTypeManager().CanCreateOperatorNode(wildcardNodeType.mNodeRecipe.mOperatorTrait, aDataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, wildcardNodeType.mNodeRecipe.mOperatorTrait, aDataTypeID, aCommandTracker);
			const Node& wildcardNode = aNodeGraph.mNodes.at(wildcardNodeID);
			SetNodePosition(createdNodeID, wildcardNode.mPosition, aNodeGraph, aCommandTracker);
			DestroyNode(aNodeGraph, wildcardNodeID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph)
		{
			return static_cast<NodeID>(ScriptProxy::GetNodes(aNodeGraph).size());
		}
	}
}
