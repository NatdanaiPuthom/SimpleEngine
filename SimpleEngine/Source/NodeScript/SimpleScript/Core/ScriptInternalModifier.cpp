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
#include "Global/ScriptGlobal.hpp"
#include "ScriptFlow.hpp"

namespace SCR
{

	namespace Internal
	{


		CustomEventID CreateCustomEvent(std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateCustomEvent(aName);
		}

		FunctionID CreateFunction(std::string_view aName)
		{
			return Global::GetNodeTypeManager().CreateFunction(aName);
		}

		NodeID CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, CommandTracker* aCommandTracker)
		{
			const NodeID id = GetCurrentNodeID(aNodeGraph);
			AddNode(aNodeGraph, Global::GetNodeTypeManager().CreateNode(aNodeGraph, id, aNodeTypeID), id, aCommandTracker);
			return id;
		}

		NodeID CreateNode(NodeGraph& aNodeGraph, std::string_view aName, bool& aSuccess, bool aCreateIfNameNotFound, CommandTracker* aCommandTracker)
		{
			const NodeTypeID typeID = Global::GetNodeTypeManager().GetTypeID(aName);
			aSuccess = typeID != 0;
			if (!aCreateIfNameNotFound && !aSuccess)
			{
				return InvalidID<NodeID>();
			}
			return CreateNode(aNodeGraph, typeID, aCommandTracker);
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

		VarID CreateVariable(Class& aScript, DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			std::vector<Variable>& variables = ScriptProxy::GetVariablesRef(aScript);
			const VarID id = variables.size();
			variables.emplace_back();
			SetVariableDataType(id, aDataTypeID, aScript, aCommandTracker);
			return id;
		}

		void BindVariable(Class& aScript, const NodeRef& aNodeRef, const VarID aVarID, CommandTracker* aCommandTracker)
		{
			struct BindVarData
			{
				NodeRef nodeRef;
				VarID varID = InvalidID<VarID>();
				Class* script = nullptr;
			} data;

			data.nodeRef = aNodeRef;
			data.varID = aVarID;
			data.script = &aScript;

			auto commandFunction = [data](eCommandType aCommandType) -> void
				{
					if (aCommandType == eCommandType::Do)
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script)[data.nodeRef] = data.varID;
					}
					else
					{
						ScriptProxy::GetNodeRefToVarIDMap(*data.script).erase(data.nodeRef);
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

		void UnbindVariable(Class& aScript, const NodeRef& aNodeRef, CommandTracker* aCommandTracker)
		{
			if (!ScriptProxy::GetNodeRefToVarIDMap(aScript).contains(aNodeRef))
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
			data.varID = ScriptProxy::GetNodeRefToVarIDMap(aScript).at(data.nodeRef);
			data.script = &aScript;

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

		void ReplaceOperatorNode(NodeGraph& aNodeGraph, PinID aReplacePinID, PinID aConnectedPinID, CommandTracker* aCommandTracker)
		{
			const Pin& replacePin = ScriptProxy::GetPin(aNodeGraph, aReplacePinID);
			const Pin& connectedPin = ScriptProxy::GetPin(aNodeGraph, aConnectedPinID);

			const NodeID replaceNodeID = replacePin.mNodeID;
			Node& replaceNode = ScriptProxy::GetNodeRef(aNodeGraph, replaceNodeID);

			const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(replaceNode.mTypeID);

			if (nodeType.mNodeRecipe.mOperatorTrait != eNodeOperatorTrait::None)
			{
				const PinType& replacePinType = Global::GetPinTypeManager().GetPinType(replacePin.mTypeID);
				const PinType& connectedPinType = Global::GetPinTypeManager().GetPinType(connectedPin.mTypeID);
				const bool canReplace = Global::GetNodeTypeManager().CanCreateOperatorNode(nodeType.mNodeRecipe.mOperatorTrait, connectedPinType.mDataTypeID);
				if (!canReplace)
				{
					return;
				}
				if (aCommandTracker)
				{
					aCommandTracker->BeginComposite("Replace node composite");
				}
				const NodeID createdNodeID = CreateOperatorNode(aNodeGraph, nodeType.mNodeRecipe.mOperatorTrait, connectedPinType.mDataTypeID, aCommandTracker);


				DestroyNode(replaceNodeID, aNodeGraph, aCommandTracker);

				Node& createdNode = ScriptProxy::GetNodeRef(aNodeGraph, createdNodeID);
				createdNode.mPosition = replaceNode.mPosition;

				Link createdLink;


				{ // Link new pin
					size_t pinIndex = ScriptLinker::GetPinIndex(aNodeGraph, aReplacePinID);

					const PinID createdPinConnectedID = replacePinType.mFlowType == eFlowType::Input ? createdNode.mInputPins[pinIndex] : createdNode.mOutputPins[pinIndex];

					TryCreateLink(aConnectedPinID, createdPinConnectedID, aNodeGraph, aCommandTracker);
				}

				{ // Link previously linked pins

					const Node& destroyedNode = ScriptProxy::GetNode(aNodeGraph, replaceNodeID);

					for (size_t pinIndex = 0; pinIndex < destroyedNode.mInputPins.size(); ++pinIndex)
					{
						const Pin& destroyedInputPin = ScriptProxy::GetPin(aNodeGraph, destroyedNode.mInputPins[pinIndex]);

						if (!destroyedInputPin.mConnectedPinIDs.empty())
						{
							TryCreateLink(destroyedInputPin.mConnectedPinIDs[0], ScriptLinker::GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Input), aNodeGraph, aCommandTracker);
						}

					}

					for (size_t pinIndex = 0; pinIndex < destroyedNode.mOutputPins.size(); ++pinIndex)
					{
						const Pin& destroyedOutputPin = ScriptProxy::GetPin(aNodeGraph, destroyedNode.mOutputPins.at(pinIndex));

						for (PinID connectedInputPinID : destroyedOutputPin.mConnectedPinIDs)
						{
							if (connectedInputPinID != InvalidID<PinID>())
							{
								TryCreateLink(connectedInputPinID, ScriptLinker::GetPinID(aNodeGraph, createdNodeID, pinIndex, eFlowType::Output), aNodeGraph, aCommandTracker);
							}
						}
					}
				}

				if (aCommandTracker)
				{
					aCommandTracker->EndComposite();
				}
			}
		}

		NodeID GetCurrentNodeID(NodeGraph& aNodeGraph)
		{
			return static_cast<NodeID>(ScriptProxy::GetNodes(aNodeGraph).size());
		}
	}
}
