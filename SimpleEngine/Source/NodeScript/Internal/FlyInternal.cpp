#include "FlyInternal.hpp"
#include "FlyClass.hpp"
#include "CustomEvent/FlyCustomEvent.hpp"
#include "Node/FlyNodeTypeManager.hpp"
#include "Pin/FlyPinTypeManager.hpp"
#include "DataType/FlyDataTypeManager.hpp"
#include "Command/FlyCommandTracker.hpp"
#include "Execution/FlyNodeExecutor.hpp"
#include "Global/FlyGlobal.hpp"
#include "FlyFlow.hpp"
#include "FlyWildcard.hpp"
#include "FlyFoundation.hpp"
#include "Node/FlyNodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	namespace Internal
	{

		NodeGraph& GetNodeGraph(const NodeGraphVariant& aNodeGraphVariant)
		{
			switch (aNodeGraphVariant.index())
			{
			case 0:
				return std::get<0>(aNodeGraphVariant)->mNodeGraph;
				break;
			case 1:
				return Global::GetNodeTypeManager().GetFunction(std::get<1>(aNodeGraphVariant).mID).mNodeGraph;
				break;
			default:
				break;
			}
			return std::get<0>(aNodeGraphVariant)->mNodeGraph;
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


		Class& CreateClass(const DataTypeID aTargetID, const std::string_view aName)
		{
			return Global::GetFoundation().CreateClass(aTargetID, aName);
		}

		CustomEventID CreateCustomEvent(const std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateCustomEvent(aName);
		}

		FunctionID CreateFunction(const std::string_view aName)
		{
			const FunctionID id = Global::GetNodeTypeManager().CreateFunction(aName);

			Function& createdFunction = Global::GetNodeTypeManager().GetFunction(id);

			createdFunction.mInputNodeID = CreateNode(FunctionIDWrapper(id), createdFunction.mInputNodeTypeID);
			createdFunction.mOutputNodeID = CreateNode(FunctionIDWrapper(id), createdFunction.mOutputNodeTypeID);

			return id;
		}

		NodeID CreateNode(const NodeGraphVariant& aNodeGraphVariant, const NodeTypeID aNodeTypeID, const Vec2 aPosition, CommandTracker* const aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Position");
			}

			NodeGraph& nodeGraph = GetNodeGraph(aNodeGraphVariant);
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

				}, aNodeGraphVariant
			);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
			return nodeID;
		}


		NodeID CreateNode(const NodeGraphVariant& aNodeGraphVariant, const std::string_view aName, bool& aSuccess, const Vec2 aPosition, bool aCreateIfNameNotFound, CommandTracker* const aCommandTracker)
		{
			const NodeTypeID typeID = Global::GetNodeTypeManager().GetTypeID(aName);
			aSuccess = typeID != 0;
			if (!aCreateIfNameNotFound && !aSuccess)
			{
				return InvalidID<NodeID>();
			}
			return CreateNode(aNodeGraphVariant, typeID, aPosition, aCommandTracker);
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

		struct TransformObjectData
		{
			float x, y;
		};

		void Do(const TransformObjectData&)
		{

		}

		void Undo(const TransformObjectData&)
		{

		}

		struct ChangeVariableData
		{
			float x, y;

			void Do() const {}
			void Undo() const {}
		};


		void AddNode(NodeGraph& aNodeGraph, Node&& aNode, const NodeID aNodeID, CommandTracker* const aCommandTracker)
		{
			std::vector<Node>& nodes = aNodeGraph.mNodes;

			nodes.emplace_back(std::move(aNode));

			Node& createdNode = nodes.back();

			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(createdNode.mTypeID);
			nodeType.mNodeRefs.push_back(CreateContextualNodeRef(aNodeID, aNodeGraph));

			struct CreateNodeData
			{
				NodeID mNodeID = InvalidID<NodeID>();
				NodeGraph* mNodeGraph = nullptr;
			} data;

			CommandNew c1(TransformObjectData{});
			CommandNew c2(ChangeVariableData{});

			CommandNew c(CreateNodeData{}, [](const CreateNodeData&) {}, [](const CreateNodeData&) {});

			data.mNodeID = aNodeID;
			data.mNodeGraph = &aNodeGraph;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					Node& node = data.mNodeGraph->mNodes.at(data.mNodeID);

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

			for (const LinkID linkID : GetLinkIDsByNode(aNodeGraph, aNodeID))
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

			void* const dataPtr = Global::GetDataTypeManager().AllocateData(dataTypeID, aNodeGraph.mMemoryArena);

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
			std::vector<Pin>& pins = aNodeGraph.mPins;
			const PinID id = static_cast<PinID>(pins.size());
			pins.push_back(Pin{ aNodeID, aPinTypeID, aDataPtr });
			return id;
		}

		void ActivateLink(NodeGraph& aNodeGraph, const LinkID aLinkID)
		{
			Link& link = aNodeGraph.mLinks[aLinkID];


			{
				Pin& inputPin = aNodeGraph.mPins.at(link.mInputPinID);
				auto it = std::find(inputPin.mConnectedPinIDs.begin(), inputPin.mConnectedPinIDs.end(), link.mOutputPinID);
				if (it == inputPin.mConnectedPinIDs.end())
				{
					inputPin.mConnectedPinIDs.push_back(link.mOutputPinID);
				}
			}
			{
				Pin& outputPin = aNodeGraph.mPins.at(link.mOutputPinID);
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
			const Link createdLink = ArePinsLinkable(aNodeGraph, aPinID1, aPinID2);
			if (!createdLink)
			{
				// Check if we can replace node with overloaded operator node
				const Pin& pin1 = aNodeGraph.mPins.at(aPinID1);
				const Pin& pin2 = aNodeGraph.mPins.at(aPinID2);

				const PinType& pinType1 = Global::GetPinTypeManager().GetPinType(pin1.mTypeID);
				const PinType& pinType2 = Global::GetPinTypeManager().GetPinType(pin2.mTypeID);

				if (pinType1.mDataTypeID == GetDataTypeID<Wildcard>())
				{
					ReplaceTemplateNodeWithLink(aNodeGraph, aPinID1, aPinID2, aCommandTracker);
				}
				else if (pinType2.mDataTypeID == GetDataTypeID<Wildcard>())
				{
					ReplaceTemplateNodeWithLink(aNodeGraph, aPinID2, aPinID1, aCommandTracker);
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


			const Pin& inputPin = aNodeGraph.mPins.at(aInputPinID);
			const Pin& outputPin = aNodeGraph.mPins.at(aOutputPinID);
			const PinType& inputPinType = Global::GetPinTypeManager().GetPinType(inputPin.mTypeID);
			const PinType& outputPinType = Global::GetPinTypeManager().GetPinType(outputPin.mTypeID);
			DataTypeID inputPinDataType = inputPinType.mDataTypeID;
			DataTypeID outputPinDataType = outputPinType.mDataTypeID;
			assert(inputPinType.mFlowType == eFlowType::Input);
			assert(outputPinType.mFlowType == eFlowType::Output);
			assert(inputPinDataType == outputPinDataType);

			if (inputPinDataType != GetDataTypeID<Flow>())
			{
				std::vector<LinkID> inputLinkIDs = GetLinkIDsByPin(aNodeGraph, aInputPinID);
				if (!inputLinkIDs.empty())
				{
					assert(inputLinkIDs.size() == 1);
					data.previousLinkID = inputLinkIDs.front();
				}
			}
			else
			{
				std::vector<LinkID> outputLinkIDs = GetLinkIDsByPin(aNodeGraph, aOutputPinID);

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
				const LinkID linkID = GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID);
				DestroyLink(aNodeGraph, linkID, aCommandTracker);
			}


			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		VarID CreateVariable(Class& aClass, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			std::vector<Variable>& variables = aClass.mStruct.mVariables;
			const VarID id = variables.size();
			variables.emplace_back();
			SetVariableDataType(aClass, id, aDataTypeID, aCommandTracker);

			for (auto& classInstance : aClass.mClassInstances)
			{
				classInstance.Get().mStructInstance.Mirror();
			}
			return id;
		}

		void SetVariableDataType(Class& aClass, const VarID aVarID, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			Variable& variable = aClass.mStruct.mVariables.at(aVarID);

			void* const defaultValueDataPtr = Global::GetDataTypeManager().AllocateData(aDataTypeID, aClass.mStruct.mMemoryArena);

			variable.mDataTypeID = aDataTypeID;
			variable.mDefaultValueDataPtr = defaultValueDataPtr;

			DestroyVariableNodes(aClass, aVarID, aCommandTracker);
		}

		void SetVariableName(Class& aClass, const VarID aVarID, const std::string_view aName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			Variable& variable = aClass.mStruct.mVariables.at(aVarID);

			variable.mName = aName;
		}

		void DestroyVariableNodes(Class& aClass, const VarID aVarID, CommandTracker* const aCommandTracker)
		{
			DestroyNodes(GetNodeRefsByVariableRef(VariableRef(aVarID, aClass)), aCommandTracker);
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

		void SetPinTypeName(const PinTypeID aPinTypeID, const std::string_view aName)
		{
			PinType& pinType = Global::GetPinTypeManager().GetPinType(aPinTypeID);

			pinType.mName = aName;
		}

		static PinTypeID AddPinToNodeType(const NodeTypeID aNodeTypeID, const DataTypeID aDataTypeID, const eFlowType aFlowType, std::string_view aPinName)
		{
			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);

			const PinTypeID createdPinTypeID = Global::GetPinTypeManager().Create(aPinName, aFlowType, aDataTypeID, Global::GetDataTypeManager().GetSetPinDataInterface(aDataTypeID, aFlowType));

			std::vector<PinTypeID>& pinTypeIDs = aFlowType == eFlowType::Input ? nodeType.mNodeRecipe.mInputPinTypeIDs : nodeType.mNodeRecipe.mOutputPinTypeIDs;
			pinTypeIDs.push_back(createdPinTypeID);

			for (const NodeRef& nodeRef : nodeType.mNodeRefs)
			{
				Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
				const PinID createdPinID = CreatePin(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), createdPinTypeID);

				pinIDs.push_back(createdPinID);
			}

			return createdPinTypeID;
		}


		static void SetPinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t aIndex, const DataTypeID aDataTypeID, const eFlowType aFlowType)
		{
			NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
			PinTypeManager& pinTypeManager = Global::GetPinTypeManager();

			NodeType& nodeType = nodeTypeManager.GetNodeType(aNodeTypeID);
			std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);
			const PinTypeID oldPinTypeID = pinTypeIDs.at(aIndex);
			const PinType& oldPinType = pinTypeManager.GetPinType(oldPinTypeID);

			const PinTypeID newPinTypeID = pinTypeManager.Create(oldPinType.mName, aFlowType, aDataTypeID, Global::GetDataTypeManager().GetSetPinDataInterface(aDataTypeID, aFlowType));

			pinTypeIDs.at(aIndex) = newPinTypeID;

			const std::vector<NodeRef>& mNodeRefs = nodeType.mNodeRefs;
			for (const NodeRef& nodeRef : mNodeRefs)
			{
				Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
				const PinID createdPinID = CreatePin(nodeRef.GetNodeGraph(), nodeRef.GetNodeID(), newPinTypeID);
				pinIDs.at(aIndex) = createdPinID;
			}
		}

		static void DeletePinAtIndexNodeType(const NodeTypeID aNodeTypeID, const size_t aIndex, const eFlowType aFlowType)
		{
			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(aNodeTypeID);

			std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, nodeType.mNodeRecipe.mInputPinTypeIDs, nodeType.mNodeRecipe.mOutputPinTypeIDs);

			assert(aIndex < pinTypeIDs.size());

			pinTypeIDs.erase(pinTypeIDs.begin() + aIndex);

			for (const NodeRef& nodeRef : nodeType.mNodeRefs)
			{
				Node& node = nodeRef.GetNodeGraph().mNodes.at(nodeRef.GetNodeID());

				std::vector<PinID>& pinIDs = SelectByFlowType(aFlowType, node.mInputPins, node.mOutputPins);
				DestroyLinksByPin(nodeRef.GetNodeGraph(), pinIDs.at(aIndex), nullptr);
				pinIDs.erase(pinIDs.begin() + aIndex);

			}
		}

		void AddPinToCustomEvent(const CustomEventID aCustomEventID, const DataTypeID aDataTypeID, const std::string_view aPinName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			AddPinToNodeType(customEvent.GetExecutorTypeID(), aDataTypeID, eFlowType::Output, aPinName);
			AddPinToNodeType(customEvent.GetCallerTypeID(), aDataTypeID, eFlowType::Input, aPinName);
		}

		void SetPinDataTypeAtIndexCustomEvent(CustomEventID aCustomEventID, DataTypeID aDataTypeID, size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			SetPinAtIndexNodeType(customEvent.GetExecutorTypeID(), aIndex, aDataTypeID, eFlowType::Output);
			SetPinAtIndexNodeType(customEvent.GetCallerTypeID(), aIndex, aDataTypeID, eFlowType::Input);
		}

		void SetPinNameAtIndexCustomEvent(const CustomEventID aCustomEventID, const std::string_view aName, const size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
			const CustomEvent& customEvent = nodeTypeManager.GetCustomEvent(aCustomEventID);

			{
				const NodeType& callerNodeType = nodeTypeManager.GetNodeType(customEvent.GetCallerTypeID());

				SetPinTypeName(callerNodeType.mNodeRecipe.mInputPinTypeIDs.at(aIndex), aName);
			}

			{
				const NodeType& executorNodeType = nodeTypeManager.GetNodeType(customEvent.GetExecutorTypeID());

				SetPinTypeName(executorNodeType.mNodeRecipe.mOutputPinTypeIDs.at(aIndex), aName);
			}
		}

		void DeletePinAtIndexCustomEvent(const CustomEventID aCustomEventID, const size_t aIndex, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}

			const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(aCustomEventID);

			DeletePinAtIndexNodeType(customEvent.GetCallerTypeID(), aIndex, eFlowType::Input);
			DeletePinAtIndexNodeType(customEvent.GetExecutorTypeID(), aIndex, eFlowType::Output);
		}

		void AddPinToFunction(const FunctionID aFunctionID, const DataTypeID aDataTypeID, const eFlowType aFlowType, std::string_view aPinName, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
			const Function& function = nodeTypeManager.GetFunction(aFunctionID);


			AddPinToNodeType(function.mCallerNodeTypeID, aDataTypeID, aFlowType, aPinName);

			const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
			AddPinToNodeType(inputOutputNodeTypeID, aDataTypeID, InvertFlowType(aFlowType), aPinName);
		}

		void SetPinDataTypeAtIndexFunction(const FunctionID aFunctionID, const DataTypeID aDataTypeID, const size_t aIndex, const eFlowType aFlowType, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionID);

			SetPinAtIndexNodeType(function.mCallerNodeTypeID, aIndex, aDataTypeID, aFlowType);

			const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
			SetPinAtIndexNodeType(inputOutputNodeTypeID, aIndex, aDataTypeID, InvertFlowType(aFlowType));
		}

		void SetPinNameAtIndexFunction(const FunctionID aFunctionID, const std::string_view aName, const size_t aIndex, const eFlowType aFlowType, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			if (aIndex == 0)
			{
				return;
			}
			const NodeTypeManager& nodeTypeManager = Global::GetNodeTypeManager();
			const Function& function = nodeTypeManager.GetFunction(aFunctionID);

			{
				const NodeType& callerNodeType = nodeTypeManager.GetNodeType(function.mCallerNodeTypeID);

				const std::vector<PinTypeID>& callerPinTypeIDs = SelectByFlowType(aFlowType, callerNodeType.mNodeRecipe.mInputPinTypeIDs, callerNodeType.mNodeRecipe.mOutputPinTypeIDs);
				SetPinTypeName(callerPinTypeIDs.at(aIndex), aName);
			}

			{
				const NodeType& inputNodeType = nodeTypeManager.GetNodeType(function.mInputNodeTypeID);
				const NodeType& outputNodeType = nodeTypeManager.GetNodeType(function.mOutputNodeTypeID);

				const std::vector<PinTypeID>& inputOutputPinTypeIDs = SelectByFlowType(aFlowType, inputNodeType.mNodeRecipe.mOutputPinTypeIDs, outputNodeType.mNodeRecipe.mInputPinTypeIDs);
				SetPinTypeName(inputOutputPinTypeIDs.at(aIndex), aName);
			}
		}

		void DeletePinAtIndexFunction(const FunctionID aFunctionID, const size_t aIndex, const eFlowType aFlowType, [[maybe_unused]] CommandTracker* const aCommandTracker)
		{
			const Function& function = Global::GetNodeTypeManager().GetFunction(aFunctionID);

			DeletePinAtIndexNodeType(function.mCallerNodeTypeID, aIndex, aFlowType);

			const NodeTypeID inputOutputNodeTypeID = SelectByFlowType(aFlowType, function.mInputNodeTypeID, function.mOutputNodeTypeID);
			DeletePinAtIndexNodeType(inputOutputNodeTypeID, aIndex, InvertFlowType(aFlowType));
		}

		void ReplaceTemplateNodeWithLink(NodeGraph& aNodeGraph, const PinID aWildcardPinID, const PinID aConnectedPinID, CommandTracker* const aCommandTracker)
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
				const size_t pinIndex = GetPinIndex(aNodeGraph, aWildcardPinID);

				const PinID createdPinConnectedID = SelectByFlowType(wildcardPinType.mFlowType, createdNode.mInputPins.at(pinIndex), createdNode.mOutputPins.at(pinIndex));

				TryCreateLink(aNodeGraph, aConnectedPinID, createdPinConnectedID, aCommandTracker);
			}

			{ // Link previously linked pins


				for (size_t pinIndex = 0; pinIndex < replacedNode.mInputPins.size(); ++pinIndex)
				{
					const Pin& destroyedInputPin = aNodeGraph.mPins.at(replacedNode.mInputPins.at(pinIndex));

					if (!destroyedInputPin.mConnectedPinIDs.empty())
					{
						TryCreateLink(aNodeGraph, destroyedInputPin.mConnectedPinIDs[0], GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Input), aCommandTracker);
					}

				}

				for (size_t pinIndex = 0; pinIndex < replacedNode.mOutputPins.size(); ++pinIndex)
				{
					const Pin& destroyedOutputPin = aNodeGraph.mPins.at(replacedNode.mOutputPins.at(pinIndex));

					for (const PinID connectedInputPinID : destroyedOutputPin.mConnectedPinIDs)
					{
						if (connectedInputPinID != InvalidID<PinID>())
						{
							TryCreateLink(aNodeGraph, connectedInputPinID, GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Output), aCommandTracker);
						}
					}
				}
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

		}

		void ReplaceTemplateNode(NodeGraph& aNodeGraph, const NodeID aReplacedNodeID, const DataTypeID aDataTypeID, CommandTracker* const aCommandTracker)
		{
			const NodeType& replacedNodeType = Global::GetNodeTypeManager().GetNodeType(aNodeGraph.mNodes.at(aReplacedNodeID).mTypeID);

			if (replacedNodeType.mNodeRecipe.mOperatorTrait == eNodeOperatorTrait::None)
			{
				return;
			}

			const bool canReplace = Global::GetNodeTypeManager().CanCreateOperatorNode(replacedNodeType.mNodeRecipe.mOperatorTrait, aDataTypeID);
			if (!canReplace)
			{
				return;
			}

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Replace node composite");
			}

			const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, replacedNodeType.mNodeRecipe.mOperatorTrait, aDataTypeID, aCommandTracker);
			const Node& replacedNode = aNodeGraph.mNodes.at(aReplacedNodeID);
			SetNodePosition(createdNodeID, replacedNode.mPosition, aNodeGraph, aCommandTracker);
			DestroyNode(aNodeGraph, aReplacedNodeID, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph)
		{
			return static_cast<NodeID>(aNodeGraph.mNodes.size());
		}

		std::vector<PinID> GetInputPins(const NodeGraph& aNodeGraph, const bool aIncludeDestroyed)
		{
			std::vector<PinID> pinIDs;
			pinIDs.reserve(aNodeGraph.mPins.size());

			for (PinID i = 0; i < aNodeGraph.mPins.size(); i++)
			{
				const Pin& pin = aNodeGraph.mPins.at(i);
				if (Global::GetPinTypeManager().GetPinType(pin.mTypeID).mFlowType != eFlowType::Input)
				{
					continue;
				}

				if (!aIncludeDestroyed && aNodeGraph.mNodes.at(pin.mNodeID).mIsDestroyed)
				{
					continue;
				}

				pinIDs.push_back(i);
			}
			return pinIDs;
		}

		std::vector<PinID> GetOutputPins(const NodeGraph& aNodeGraph, const bool aIncludeDestroyed)
		{
			std::vector<PinID> pinIDs;
			pinIDs.reserve(aNodeGraph.mPins.size());

			for (PinID i = 0; i < aNodeGraph.mPins.size(); i++)
			{
				const Pin& pin = aNodeGraph.mPins.at(i);
				if (Global::GetPinTypeManager().GetPinType(pin.mTypeID).mFlowType != eFlowType::Output)
				{
					continue;
				}

				if (!aIncludeDestroyed && aNodeGraph.mNodes.at(pin.mNodeID).mIsDestroyed)
				{
					continue;
				}

				pinIDs.push_back(i);
			}
			return pinIDs;
		}

		VariableRef GetVariableRefByNodeRef(const GlobalNodeRef& aNodeRef)
		{
			return Global::GetFoundation().GetVariableRefByNodeRef(aNodeRef);
		}

		std::vector<GlobalNodeRef> GetNodeRefsByVariableRef(const VariableRef& aVarRef)
		{
			return Global::GetFoundation().GetNodeRefsByVariableRef(aVarRef);
		}

		PinID GetPinID(const NodeGraph& aNodeGraph, const NodeID aNodeID, const size_t aPinIndex, const eFlowType aPinFlowType)
		{
			const Node& node = aNodeGraph.mNodes.at(aNodeID);
			switch (aPinFlowType)
			{
			case eFlowType::Input:
				if (aPinIndex < node.mInputPins.size())
				{
					return node.mInputPins.at(aPinIndex);
				}
				break;
			case eFlowType::Output:
				if (aPinIndex < node.mOutputPins.size())
				{
					return node.mOutputPins.at(aPinIndex);
				}
				break;
			default:
				break;
			}
			return InvalidID<PinID>();
		}

		size_t GetPinIndex(const NodeGraph& aNodeGraph, const PinID aPinID)
		{
			const Pin& pin = aNodeGraph.mPins.at(aPinID);
			const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);
			const Node& node = aNodeGraph.mNodes.at(pin.mNodeID);

			const std::vector<PinID>& pinIDs = pinType.mFlowType == eFlowType::Output ? node.mOutputPins : node.mInputPins;

			for (size_t i = 0; i < pinIDs.size(); ++i)
			{
				if (pinIDs[i] == aPinID)
				{
					return i;
				}
			}
			return InvalidID<size_t>();
		}

		PinID GetOpposingPinID(const NodeGraph& aPreviousNodeGraph, const PinID aPreviousPinID, const NodeGraph& aNewNodeGraph, const NodeID aNodeID)
		{
			const size_t pinIndex = GetPinIndex(aPreviousNodeGraph, aPreviousPinID);
			const Pin& pin = aPreviousNodeGraph.mPins.at(aPreviousPinID);
			const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);
			return GetPinID(aNewNodeGraph, aNodeID, pinIndex, pinType.mFlowType);
		}

		static bool ArePinsLinkableByDataType(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2)
		{
			const Pin& pin1 = aNodeGraph.mPins.at(aPinID1);
			const Pin& pin2 = aNodeGraph.mPins.at(aPinID2);
			return  Global::GetPinTypeManager().GetPinType(pin1.mTypeID).mDataTypeID == Global::GetPinTypeManager().GetPinType(pin2.mTypeID).mDataTypeID;
		}

		Link ArePinsLinkable(const NodeGraph& aNodeGraph, PinID aPinID1, PinID aPinID2)
		{
			const Pin& pin1 = aNodeGraph.mPins.at(aPinID1);
			const Pin& pin2 = aNodeGraph.mPins.at(aPinID2);
			const PinType& pinType1 = Global::GetPinTypeManager().GetPinType(pin1.mTypeID);
			const PinType& pinType2 = Global::GetPinTypeManager().GetPinType(pin2.mTypeID);

			Link outputLink;

			switch (pinType1.mFlowType)
			{
			case eFlowType::Input:
				if (pinType2.mFlowType == eFlowType::Output)
				{
					if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
					{
						outputLink.mInputPinID = aPinID1;
						outputLink.mOutputPinID = aPinID2;
					}
				}
				break;
			case eFlowType::Output:
				if (pinType2.mFlowType == eFlowType::Input)
				{
					if (ArePinsLinkableByDataType(aNodeGraph, aPinID1, aPinID2))
					{
						outputLink.mInputPinID = aPinID2;
						outputLink.mOutputPinID = aPinID1;
					}
				}
				break;
			default:
				break;
			}
			return outputLink;
		}

		LinkID GetLinkIDByPinIDs(const NodeGraph& aNodeGraph, const PinID aPinID1, const PinID aPinID2, bool aIncludeDestroyed)
		{
			for (LinkID id = 0; id < aNodeGraph.mLinks.size(); ++id)
			{
				const Link& link = aNodeGraph.mLinks[id];
				if (!aIncludeDestroyed && link.mIsDestroyed)
				{
					continue;
				}
				if (link == Link{ aPinID1, aPinID2 } || link == Link{ aPinID2, aPinID1 })
				{
					return id;
				}
			}
			return InvalidID<LinkID>();
		}

		std::vector<LinkID> GetLinkIDsByPin(const NodeGraph& aNodeGraph, const PinID aPinID, bool aIncludeDestroyed)
		{
			std::vector<LinkID> linkIDs;
			const Pin& pin = aNodeGraph.mPins.at(aPinID);

			for (const PinID connectedPinID : pin.mConnectedPinIDs)
			{
				const LinkID linkID = GetLinkIDByPinIDs(aNodeGraph, aPinID, connectedPinID, aIncludeDestroyed);
				assert(linkID != InvalidID<LinkID>());

				linkIDs.push_back(linkID);
			}

			return linkIDs;
		}

		std::vector<LinkID> GetLinkIDsByNode(const NodeGraph& aNodeGraph, const NodeID aNodeID)
		{
			std::vector<LinkID> linkIDs;
			const Node& node = aNodeGraph.mNodes.at(aNodeID);

			for (const PinID inputPinID : node.mInputPins)
			{
				std::vector<LinkID> inputLinks = GetLinkIDsByPin(aNodeGraph, inputPinID);
				linkIDs.insert(linkIDs.end(), inputLinks.begin(), inputLinks.end());
			}
			for (const PinID mOutputPinID : node.mOutputPins)
			{
				std::vector<LinkID> outputLinks = GetLinkIDsByPin(aNodeGraph, mOutputPinID);
				linkIDs.insert(linkIDs.end(), outputLinks.begin(), outputLinks.end());
			}


			return linkIDs;
		}
	}
}
