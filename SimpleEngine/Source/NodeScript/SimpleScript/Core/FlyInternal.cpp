#include "FlyInternal.hpp"
#include "FlyClass.hpp"
#include "CustomEvent/FlyCustomEvent.hpp"
#include "Node/FlyNodeTypeManager.hpp"
#include "Pin/FlyPinTypeManager.hpp"
#include "DataType/FlyDataTypeManager.hpp"
#include "Command/FlyCommandTracker.hpp"
#include "Execution/FlyNodeExecutor.hpp"
#include "Utilities/FlyLinker.hpp"
#include "Global/FlyGlobal.hpp"
#include "FlyFlow.hpp"
#include "FlyWildcard.hpp"
#include "Utilities/FlyProxy.hpp"
#include "FlyFoundation.hpp"
#include "Node/FlyNodeTypeRegistry.hpp"

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


		CustomEventID CreateCustomEvent(const std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateCustomEvent(aName);
		}

		FunctionID CreateFunction(const std::string_view aName)
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

		NodeID CreateGetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, Global::GetNodeTypeManager().CreateGetterNode(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateSetterNode(NodeGraph& aNodeGraph, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, Global::GetNodeTypeManager().CreateSetterNode(aNodeGraph, id, aDataTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateOperatorNode(NodeGraph& aNodeGraph, const eNodeOperatorTrait aOperatorTrait, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, Global::GetNodeTypeManager().CreateOperatorNode(aNodeGraph, id, aOperatorTrait, aDataTypeID), id, aCommandTracker);
			return id;
		}

		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* const aCommandTracker)
		{
			std::vector<Node>& nodes = ScriptProxy::GetNodes(aNodeGraph);

			nodes.emplace_back(std::move(aNode));

			Node& createdNode = nodes.back();

			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(createdNode.mTypeID);
			nodeType.mNodeRefs.push_back(CreateContextualNodeRef(aNodeID, aNodeGraph));

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

		void DestroyNodes(const std::vector<NodeRef>& aNodeRefs, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Nodes");
			}

			for (const NodeRef& nodeRef : aNodeRefs)
			{
				DestroyNode(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroyNodes(const std::vector<GlobalNodeRef>& aNodeRefs, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Nodes");
			}

			for (const GlobalNodeRef& nodeRef : aNodeRefs)
			{
				DestroyNode(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void SetNodePosition(const NodeID aNodeID, const Vec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
		{
			const Vec2 oldPos = aNodeGraph.mNodes.at(aNodeID).mPosition;
			SetNodePosition(aNodeID, aPosition, oldPos, aNodeGraph, aCommandTracker);
		}

		void SetNodePosition(const NodeID aNodeID, const Vec2 aPosition, const Vec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* const aCommandTracker)
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

			void* const dataPtr = Global::GetDataTypeManager().AllocateData(dataTypeID, memoryArena);

			return CreatePin(aNodeGraph, aNodeID, aPinTypeID, dataPtr);
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

			Pin& inputPin = aNodeGraph.mPins.at(link.mInputPinID);
			Pin& outputPin = aNodeGraph.mPins.at(link.mOutputPinID);

			std::erase(inputPin.mConnectedPinIDs, link.mOutputPinID);
			std::erase(outputPin.mConnectedPinIDs, link.mInputPinID);

			link.mIsDestroyed = true;
		}

		LinkID TryCreateLink(NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2, CommandTracker* const aCommandTracker)
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

		LinkID CreateLink(NodeGraph& aNodeGraph, const PinID aInputPinID, const PinID aOutputPinID, CommandTracker* const aCommandTracker)
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


		void DestroyLink(NodeGraph& aNodeGraph, const LinkID aLinkID, CommandTracker* const aCommandTracker)
		{
			assert(aLinkID != InvalidID<LinkID>());

			struct DestroyLinkData
			{
				LinkID mDestroyedLinkID = InvalidID<LinkID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			data.mDestroyedLinkID = aLinkID;
			data.mNodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					void (*func) (NodeGraph&, LinkID) = aCommandType == eCommandType::Do ? DeactivateLink : ActivateLink;

					func(*data.mNodeGraph, data.mDestroyedLinkID);
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

		void DestroyLinksByPin(NodeGraph& aNodeGraph, const PinID aPinID, CommandTracker* const aCommandTracker)
		{
			const Pin& pin = aNodeGraph.mPins.at(aPinID);

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Links By Pin");
			}


			for (const PinID connectedPinID : pin.mConnectedPinIDs)
			{
				const LinkID linkID = ScriptLinker::GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID);
				DestroyLink(aNodeGraph, linkID, aCommandTracker);
			}


			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		VarID CreateVariable(Class& aClass, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			std::vector<Variable>& variables = aClass.mVariableManager.mVariables;
			const VarID id = variables.size();
			variables.emplace_back();
			SetVariableDataType(aClass, id, aDataTypeID, aCommandTracker);
			return id;
		}

		void SetVariableDataType(Class& aClass, const VarID aVarID, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			Variable& variable = aClass.mVariableManager.mVariables.at(aVarID);

			void* defaultValueDataPtr = Global::GetDataTypeManager().AllocateData(aDataTypeID, aClass.mVariableManager.mMemoryArena);

			variable.mDataTypeID = aDataTypeID;
			variable.mDefaultValueDataPtr = defaultValueDataPtr;

			DestroyVariableNodes(aClass, aVarID, aCommandTracker);
		}

		void DestroyVariableNodes(Class& aClass, const VarID aVarID, CommandTracker* const aCommandTracker)
		{
			Internal::DestroyNodes(GetNodeRefsByVariableRef(VariableRef(aVarID, aClass)), aCommandTracker);
		}

		void BindVariable(Class& aClass, const NodeRef& aNodeRef, const VarID aVarID, CommandTracker* const aCommandTracker)
		{
			const GlobalNodeRef gNodeRef = CreateGlobalNodeRef(aNodeRef, aClass);
			struct BindVarData
			{
				GlobalNodeRef mNodeRef;
				VariableRef mVarRef;
			} data;

			data.mNodeRef = gNodeRef;
			data.mVarRef = VariableRef(aVarID, aClass);

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						Global::GetFoundation().mNodeRefToVarRef[data.mNodeRef] = data.mVarRef;
					}
					else
					{
						Global::GetFoundation().mNodeRefToVarRef.erase(data.mNodeRef);
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
			const GlobalNodeRef gNodeRef = CreateGlobalNodeRef(aNodeRef, aClass);
			if (!Global::GetFoundation().mNodeRefToVarRef.contains(gNodeRef))
			{
				return;
			}

			struct UnbindVarData
			{
				GlobalNodeRef mNodeRef;
				VariableRef mVarRef;
			} data;

			data.mNodeRef = gNodeRef;
			data.mVarRef = GetVariableRefByNodeRef(gNodeRef);

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						Global::GetFoundation().mNodeRefToVarRef.erase(data.mNodeRef);
					}
					else
					{
						Global::GetFoundation().mNodeRefToVarRef[data.mNodeRef] = data.mVarRef;
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

		VariableRef GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef)
		{
			return Global::GetFoundation().GetVariableRefByNodeRef(aNodeRef);
		}

		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef)
		{
			return Global::GetFoundation().GetNodeRefsByVariableRef(aVarRef);
		}
	}
}
