#include "ScriptModifier.h"
#include "Script.h"
#include "Node/NodeTypeManager.h"
#include "Pin/PinTypeManager.h"
#include "Node/NodeTypeRegistry.h"
#include "ScriptManager.h"
#include "ScriptInternalModifier.h"
#include "ScriptCopyBuffer.h"
#include "Command/ScriptFunctionCommand.h"
#include "Utilities/ScriptLinker.h"
#include "Command/ScriptCommandTracker.h"
#include "ScriptFoundation.h"

namespace SCR
{
	namespace Modify
	{


		NodeID CreateNode(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, ScriptVec2 aPosition, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Node Pos");
			}

			NodeID nodeID = InternalModifier::CreateNode(aNodeGraph, aNodeTypeID, aCommandTracker);

			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;
		}

		NodeID CreateNodeAutoLink(NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID, PinID aConnection, ScriptVec2 aPosition, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Auto Link");
			}

			NodeID nodeID = CreateNode(aNodeGraph, aNodeTypeID, aPosition, aCommandTracker);

			const Pin& createdFromPin = ScriptProxy::GetPin(aNodeGraph, aConnection);
			const Node& createdNode = ScriptProxy::GetNode(aNodeGraph, nodeID);

			const std::vector<PinID>& pinIDs = PinTypeManager::GetPinType(createdFromPin.typeID).flowType == ePinFlowType::Input ? createdNode.outputPins : createdNode.inputPins;

			for (PinID pinID : pinIDs)
			{
				if (TryCreateLink(pinID, aConnection, aNodeGraph, aCommandTracker))
				{
					break;
				}
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;
		}

		NodeID CreateNode(NodeGraph& aNodeGraph, const std::string& aName, bool& aSuccess, ScriptVec2 aPosition, CommandTracker* aCommandTracker, bool aCreateIfNameNotFound)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Node + Set Node Pos");
			}

			NodeID nodeID = InternalModifier::CreateNode(aNodeGraph, aName, aSuccess, aCreateIfNameNotFound, aCommandTracker);

			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

			return nodeID;
		}

		NodeID CreateGetterNode(Script& aScript, NodeGraph& aNodeGraph, DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Getter Node");
			}

			NodeID nodeID = InternalModifier::CreateGetterNode(aNodeGraph, aDataTypeID, aCommandTracker);
			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);
			InternalModifier::BindVariable(aScript, nodeID, aVarID);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
			return nodeID;
		}

		NodeID CreateSetterNode(Script& aScript, NodeGraph& aNodeGraph, DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Create Setter Node");
			}

			NodeID nodeID = InternalModifier::CreateSetterNode(aNodeGraph, aDataTypeID, aCommandTracker);
			SetNodePosition(nodeID, aPosition, aNodeGraph, aCommandTracker);
			InternalModifier::BindVariable(aScript, nodeID, aVarID);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
			return nodeID;
		}

		Link TryCreateLink(PinID aPinID1, PinID aPinID2, /*Script& aScript, */NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			Link createdLink = ScriptLinker::ArePinsLinkable(aNodeGraph, aPinID1, aPinID2);
			if (!createdLink)
			{
				// Check if we can replace node with overloaded opearator node
				const Pin& pin1 = ScriptProxy::GetPin(aNodeGraph, aPinID1);
				const Pin& pin2 = ScriptProxy::GetPin(aNodeGraph, aPinID2);

				const PinType& pinType1 = PinTypeManager::GetPinType(pin1.typeID);
				const PinType& pinType2 = PinTypeManager::GetPinType(pin2.typeID);

				if (pinType1.dataTypeID == typeid(Wildcard).hash_code())
				{
					return InternalModifier::ReplaceOperatorNode(/*aScript, */aNodeGraph, aPinID1, aPinID2, aCommandTracker);

				}
				else if (pinType2.dataTypeID == typeid(Wildcard).hash_code())
				{
					return InternalModifier::ReplaceOperatorNode(/*aScript, */aNodeGraph, aPinID2, aPinID1, aCommandTracker);
				}
				else
				{
					return Link{};
				}
			}

			InternalModifier::RebindLink(aNodeGraph, createdLink.inputPinID, createdLink.outputPinID, aCommandTracker);


			return Link{ aPinID2, aPinID1 };
		}

		void DestroyLink(const PinID aInputPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			InternalModifier::RebindLink(aNodeGraph, aInputPinID, InvalidID<PinID>(), aCommandTracker);
		}

		void DestoryLinksByOutputPinID(const PinID aOutputPinID, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			Pin& outputPin = aNodeGraph.myPinManager->myPins[aOutputPinID];

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destory Links by Output Pin");
			}
			for (PinID connectedInputPin : outputPin.connectedPinIDs)
			{
				DestroyLink(connectedInputPin, aNodeGraph, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroyNode(const NodeID aNodeID, /*Script& aScript, */NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			struct DestroyNodeData
			{
				NodeID nodeID = InvalidID<NodeID>();
			} data;

			data.nodeID = aNodeID;

			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Node + Conncected Links");

			}

			auto doAction = [](const DestroyNodeData& aData, const CommandContext& aContext) -> void
				{
					Node& node = aContext.nodeGraph->myNodeManager->myNodes[aData.nodeID];
					node.isDestroyed = true;
					ScriptProxy::GetNodeExecutor().UnbindFromEvent(NodeRef{ aData.nodeID, aContext.nodeGraph });
				};

			if (!aCommandTracker)
			{
				doAction(data, CommandContext{ .nodeGraph = &aNodeGraph });
			}
			else
			{
				aCommandTracker->DoCommand<FunctionCommand<DestroyNodeData>>(CommandContext{ .nodeGraph = &aNodeGraph }, data,
					doAction,
					[](const DestroyNodeData& aData, const CommandContext& aContext) -> void
					{
						Node& node = aContext.nodeGraph->myNodeManager->myNodes[aData.nodeID];
						node.isDestroyed = false;

						ScriptProxy::GetNodeExecutor().BindToEvent(NodeRef{ aData.nodeID, aContext.nodeGraph });
					}, "Destroy Node"
				);
			}

			//InternalModifier::UnbindVariable(aScript, aNodeID);

			for (PinID inputPinID : ScriptProxy::GetNode(aNodeGraph, aNodeID).inputPins)
			{
				DestroyLink(inputPinID, aNodeGraph, aCommandTracker);
			}

			for (PinID outputPinID : ScriptProxy::GetNode(aNodeGraph, aNodeID).outputPins)
			{
				const Pin& outputPin = ScriptProxy::GetPin(aNodeGraph, outputPinID);

				for (PinID connectedInputPin : outputPin.connectedPinIDs)
				{
					DestroyLink(connectedInputPin, aNodeGraph, aCommandTracker);
				}
			}


			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Selection");
			}

			for (LinkID linkID : aLinkIDs)
			{
				DestroyLink(linkID, aNodeGraph, aCommandTracker);
			}

			for (NodeID nodeID : aNodeIDs)
			{
				DestroyNode(nodeID, aNodeGraph, aCommandTracker);
			}

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}
		}

		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			SetNodePosition(aNodeID, aPosition, aNodeGraph.myNodeManager->myNodes.at(aNodeID).position, aNodeGraph, aCommandTracker);
		}

		void SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, ScriptVec2 aOldPosition, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			if (!aCommandTracker)
			{
				aNodeGraph.myNodeManager->myNodes.at(aNodeID).position = aPosition;
			}

			struct SetNodePositionData
			{
				NodeID nodeID = InvalidID<NodeID>();
				ScriptVec2 oldPos;
				ScriptVec2 newPos;
			} data;

			if (aPosition != aOldPosition)
			{

				data.nodeID = aNodeID;
				data.oldPos = aOldPosition;
				data.newPos = aPosition;

				aCommandTracker->DoCommand<FunctionCommand<SetNodePositionData>>(CommandContext{ nullptr, &aNodeGraph }, data,
					[](const SetNodePositionData& aData, const CommandContext& aContext) -> void
					{
						Node& node = aContext.nodeGraph->myNodeManager->myNodes.at(aData.nodeID);
						node.position = aData.newPos;
					},
					[](const SetNodePositionData& aData, const CommandContext& aContext) -> void
					{
						Node& node = aContext.nodeGraph->myNodeManager->myNodes.at(aData.nodeID);
						node.position = aData.oldPos;
					}, "Set Node Position"
				);
			}
		}

		VarID CreateVariable(Script& aScript, DataTypeID aDataTypeID, CommandTracker* aCommandTracker)
		{
			std::vector<Variable>& variables = ScriptProxy::GetVariablesRef(aScript);
			VarID id = variables.size();
			variables.emplace_back();
			SetVariableDataType(id, aDataTypeID, aScript, aCommandTracker);
			return id;
		}

		void DestroyVariable(VarID aVarID, Script& aScript, CommandTracker* aCommandTracker)
		{
			if (aCommandTracker)
			{
				aCommandTracker->BeginComposite("Destroy Variable + Connected Nodes");
			}

			struct DestroyVariableData
			{
				VarID varID = InvalidID<VarID>();
			} data;

			data.varID = aVarID;

			auto doAction = [](const DestroyVariableData& aData, const CommandContext& aContext) -> void
				{
					ScriptProxy::GetVariableRef(*aContext.script, aData.varID).isDestroyed = true;
				};

			CommandContext commandContext{ .script = &aScript };

			if (!aCommandTracker)
			{
				doAction(data, commandContext);
			}
			else
			{
				aCommandTracker->DoCommand<FunctionCommand<DestroyVariableData>>(commandContext, data,
					doAction,
					[](const DestroyVariableData& aData, const CommandContext& aContext) -> void
					{
						ScriptProxy::GetVariableRef(*aContext.script, aData.varID).isDestroyed = false;
					}
				);
			}

			const VariableManager& variableManager = ScriptProxy::GetVariableManager(aScript);
			DestroySelection(variableManager.GetNodeIDsByVarID(aVarID), {}, aScript.GetEventGraph(), aCommandTracker);

			if (aCommandTracker)
			{
				aCommandTracker->EndComposite();
			}

		}

		void EditPin(PinID aPinID, NodeGraph& aNodeGraph, CommandTracker*)
		{
			const Pin& pin = aNodeGraph.myPinManager->myPins[aPinID];
			const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

			if (DataTypeManager::EditData(pinType.dataTypeID, pin.dataPtr))
			{

			}
		}

		void EditVariableDefaultValue(VarID aVarID, Script& aScript, CommandTracker*)
		{
			const Variable& variable = ScriptProxy::GetVariable(aScript, aVarID);

			if (DataTypeManager::EditData(variable.dataTypeID, variable.defaultValueDataPtr))
			{

			}
		}

		void SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID, Script& aScript, CommandTracker* aCommandTracker)
		{
			Variable& variable = ScriptProxy::GetVariableRef(aScript, aVarID);

			void* runtimeDataPtr = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryManager(aScript));
			void* defaultValueDataPtr = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryManager(aScript));

			variable.dataTypeID = aDataTypeID;
			variable.runtimeDataPtr = runtimeDataPtr;
			variable.defaultValueDataPtr = defaultValueDataPtr;

			DestroyVariableNodes(aVarID, aScript, aScript.GetEventGraph(), aCommandTracker);
		}

		void SetVariableName(VarID aVarID, const std::string& aName, Script& aScript)
		{
			Variable& variable = ScriptProxy::GetVariableRef(aScript, aVarID);

			variable.name = aName;
		}

		void DestroyVariableNodes(const VarID aVarID, Script& aScript, NodeGraph& aNodeGraph, CommandTracker* aCommandTracker)
		{
			DestroySelection(ScriptProxy::GetVariableManager(aScript).GetNodeIDsByVarID(aVarID), {}, aNodeGraph, aCommandTracker);
		}

		CustomEventID CreateCustomEvent(const std::string& aName)
		{
			return InternalModifier::CreateCustomEvent(aName);
		}

		void AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, const std::string& aName)
		{
			CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);


			NodeType& nodeType = NodeTypeManager::GetNodeType(customEvent.GetExecutorTypeID());

			PinTypeID executorPinTypeID = PinTypeManager::Create(aName, ePinFlowType::Output, aDataTypeID, CreatePinSetFunction());
			nodeType.nodeRecipe.outputPinTypeIDs.push_back(executorPinTypeID);


			NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.GetCallerTypeID());
			PinTypeID callerPinTypeID = PinTypeManager::Create(aName, ePinFlowType::Input, aDataTypeID, CreatePinSetFunction());
			callerNodeType.nodeRecipe.inputPinTypeIDs.push_back(callerPinTypeID);

			ScriptFoundation* foundation = ScriptFoundation::GetInstance();

			if (!foundation)
			{
				return;
			}

			for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*foundation))
			{
				for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
				{
					// TODO - fix
					NodeGraph& eventGraph = ScriptProxy::GetEventGraph(*script);

					for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetExecutorTypeID()))
					{
						PinID pinID = InternalModifier::CreateOutputPin(eventGraph, nodeID, executorPinTypeID);

						ScriptProxy::GetNodeRef(eventGraph, nodeID).outputPins.push_back(pinID);
					}

					for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetCallerTypeID()))
					{
						PinID pinID = InternalModifier::CreateInputPin(eventGraph, nodeID, callerPinTypeID);

						ScriptProxy::GetNodeRef(eventGraph, nodeID).inputPins.push_back(pinID);
					}
				}
			}
		}

		void SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID)
		{
			if (anIndex == 0)
			{
				return;
			}
			CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);

			NodeType& nodeType = NodeTypeManager::GetNodeType(customEvent.GetExecutorTypeID());

			if (anIndex >= nodeType.nodeRecipe.outputPinTypeIDs.size())
			{
				return;
			}

			const PinType& oldExecutorPinType = PinTypeManager::GetPinType(nodeType.nodeRecipe.outputPinTypeIDs[anIndex]);
			if (oldExecutorPinType.dataTypeID == aDataTypeID)
			{
				return;
			}

			PinTypeID newExecutorPinTypeID = PinTypeManager::Create(oldExecutorPinType.name, ePinFlowType::Output, aDataTypeID, CreatePinSetFunction());
			nodeType.nodeRecipe.outputPinTypeIDs[anIndex] = newExecutorPinTypeID;



			NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.GetCallerTypeID());

			const PinType& oldCallerPinType = PinTypeManager::GetPinType(callerNodeType.nodeRecipe.inputPinTypeIDs[anIndex]);

			PinTypeID newCallerPinTypeID = PinTypeManager::Create(oldCallerPinType.name, ePinFlowType::Input, aDataTypeID, CreatePinSetFunction());
			callerNodeType.nodeRecipe.inputPinTypeIDs[anIndex] = newCallerPinTypeID;

			ScriptFoundation* foundation = ScriptFoundation::GetInstance();

			if (!foundation)
			{
				return;
			}

			for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*foundation))
			{
				for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
				{
					// TODO - fix
					NodeGraph& eventGraph = ScriptProxy::GetEventGraph(*script);

					for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetExecutorTypeID()))
					{
						PinID pinID = InternalModifier::CreateOutputPin(eventGraph, nodeID, newExecutorPinTypeID);

						ScriptProxy::GetNodeRef(eventGraph, nodeID).outputPins[anIndex] = pinID;

					}

					for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetCallerTypeID()))
					{
						PinID pinID = InternalModifier::CreateInputPin(eventGraph, nodeID, newCallerPinTypeID);

						ScriptProxy::GetNodeRef(eventGraph, nodeID).inputPins[anIndex] = pinID;
					}
				}
			}

		}

		void DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aNodeTypeID)
		{
			if (anIndex == 0)
			{
				return;
			}

			CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);

			NodeType& executorNodeType = NodeTypeManager::GetNodeType(customEvent.GetExecutorTypeID());

			if (anIndex >= executorNodeType.nodeRecipe.outputPinTypeIDs.size())
			{
				return;
			}

			executorNodeType.nodeRecipe.outputPinTypeIDs.erase(executorNodeType.nodeRecipe.outputPinTypeIDs.begin() + anIndex);


			NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.GetCallerTypeID());
			callerNodeType.nodeRecipe.inputPinTypeIDs.erase(callerNodeType.nodeRecipe.inputPinTypeIDs.begin() + anIndex);

			ScriptFoundation* foundation = ScriptFoundation::GetInstance();

			if (!foundation)
			{
				return;
			}

			for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*foundation))
			{
				for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
				{
					// TODO
					NodeGraph& eventGraph = ScriptProxy::GetEventGraph(*script);
					for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetExecutorTypeID()))
					{

						Node& node = ScriptProxy::GetNodeRef(eventGraph, nodeID);
						script->GetModifier().DestoryLinksByOutputPinID(node.outputPins[anIndex]);

						node.outputPins.erase(node.outputPins.begin() + anIndex);
					}


					for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetCallerTypeID()))
					{
						Node& node = ScriptProxy::GetNodeRef(eventGraph, nodeID);

						script->GetModifier().DestroyLink(node.inputPins[anIndex]);
						node.inputPins.erase(node.inputPins.begin() + anIndex);
					}
				}
			}
		}

		FunctionID CreateGlobalFunction(const std::string& aName)
		{
			return InternalModifier::CreateFunction(aName);
		}

}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="aScript"></param>

	ScriptModifier::ScriptModifier(Script& aScript)
		: myScript(aScript)
	{
		myCurrentNodeGraph = &ScriptProxy::GetEventGraph(myScript);
	}

	ScriptModifier::~ScriptModifier()
	{
	}

	NodeID ScriptModifier::CreateNode(const NodeTypeID aNodeTypeID, ScriptVec2 aPosition)
	{
		//ScriptProxy::GetCommandTracker(ScriptFoundation::GetInstance()).BeginComposite();
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Node + Set Node Pos");

		NodeID nodeID = InternalModifier::CreateNode(*myCurrentNodeGraph, aNodeTypeID, &ScriptProxy::GetCommandTracker(myScript));

		SetNodePosition(nodeID, aPosition, true);

		ScriptProxy::GetCommandTracker(myScript).EndComposite();

		return nodeID;
	}

	NodeID ScriptModifier::CreateNodeAutoLink(const NodeTypeID aNodeTypeID, PinID aConnection, ScriptVec2 aPosition)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Node + Auto Link");

		NodeID nodeID = CreateNode(aNodeTypeID, aPosition);

		const Pin& createdFromPin = ScriptProxy::GetPin(*myCurrentNodeGraph, aConnection);
		const Node& createdNode = ScriptProxy::GetNode(*myCurrentNodeGraph, nodeID);

		const std::vector<PinID>& pinIDs = PinTypeManager::GetPinType(createdFromPin.typeID).flowType == ePinFlowType::Input ? createdNode.outputPins : createdNode.inputPins;

		for (PinID pinID : pinIDs)
		{
			if (TryCreateLink(pinID, aConnection))
			{
				break;
			}
		}

		ScriptProxy::GetCommandTracker(myScript).EndComposite();

		return nodeID;
	}

	NodeID ScriptModifier::CreateNode(const std::string& aName, bool& aSuccess, ScriptVec2 aPosition, bool aTrackChange, bool aCreateIfNameNotFound)
	{
		CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
		bool prevTracking = commandTracker.IsTracking();
		commandTracker.IsTracking() = aTrackChange;
		commandTracker.BeginComposite("Create Node + Set Node Pos");
		NodeID nodeID = InternalModifier::CreateNode(*myCurrentNodeGraph, aName, aSuccess, aCreateIfNameNotFound, &commandTracker);

		SetNodePosition(nodeID, aPosition, aTrackChange);

		commandTracker.EndComposite();
		commandTracker.IsTracking() = prevTracking;
		return nodeID;
	}

	NodeID ScriptModifier::CreateGetterNode(DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Getter Node");

		NodeID nodeID = InternalModifier::CreateGetterNode(*myCurrentNodeGraph, aDataTypeID, &ScriptProxy::GetCommandTracker(myScript));
		SetNodePosition(nodeID, aPosition, true);
		InternalModifier::BindVariable(myScript, nodeID, aVarID);

		ScriptProxy::GetCommandTracker(myScript).EndComposite();
		return nodeID;
	}

	NodeID ScriptModifier::CreateSetterNode(DataTypeID aDataTypeID, VarID aVarID, ScriptVec2 aPosition)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Create Setter Node");

		NodeID nodeID = InternalModifier::CreateSetterNode(*myCurrentNodeGraph, aDataTypeID, &ScriptProxy::GetCommandTracker(myScript));
		SetNodePosition(nodeID, aPosition, true);
		InternalModifier::BindVariable(myScript, nodeID, aVarID);

		ScriptProxy::GetCommandTracker(myScript).EndComposite();
		return nodeID;
	}


	Link ScriptModifier::TryCreateLink(PinID aPinID1, PinID aPinID2)
	{
		Link createdLink = ScriptLinker::ArePinsLinkable(*myCurrentNodeGraph, aPinID1, aPinID2);
		if (!createdLink)
		{
			// Check if we can replace node with overloaded opearator node
			const Pin& pin1 = ScriptProxy::GetPin(*myCurrentNodeGraph, aPinID1);
			const Pin& pin2 = ScriptProxy::GetPin(*myCurrentNodeGraph, aPinID2);

			const PinType& pinType1 = PinTypeManager::GetPinType(pin1.typeID);
			const PinType& pinType2 = PinTypeManager::GetPinType(pin2.typeID);

			if (pinType1.dataTypeID == typeid(Wildcard).hash_code())
			{
				return InternalModifier::ReplaceOperatorNode(*myCurrentNodeGraph, aPinID1, aPinID2, &ScriptProxy::GetCommandTracker(myScript));

			}
			else if (pinType2.dataTypeID == typeid(Wildcard).hash_code())
			{
				return InternalModifier::ReplaceOperatorNode(*myCurrentNodeGraph, aPinID2, aPinID1, &ScriptProxy::GetCommandTracker(myScript));
			}
			else
			{
				return Link{};
			}
		}

		InternalModifier::RebindLink(*myCurrentNodeGraph, createdLink.inputPinID, createdLink.outputPinID, &ScriptProxy::GetCommandTracker(myScript));


		return Link{ aPinID2, aPinID1 };
	}

	void ScriptModifier::DestroyLink(const PinID aInputPinID)
	{
		InternalModifier::RebindLink(*myCurrentNodeGraph, aInputPinID, InvalidID<PinID>(), &ScriptProxy::GetCommandTracker(myScript));
	}

	void ScriptModifier::DestoryLinksByOutputPinID(const PinID aOutputPinID)
	{

		Pin& outputPin = ScriptProxy::GetPinRef(*myCurrentNodeGraph, aOutputPinID);

		for (PinID connectedInputPin : outputPin.connectedPinIDs)
		{
			DestroyLink(connectedInputPin);
		}
	}

	void ScriptModifier::DestroyNode(const NodeID aNodeID)
	{

		struct DestroyNodeData
		{
			NodeID nodeID = InvalidID<NodeID>();
		} data;

		data.nodeID = aNodeID;

		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Destroy Node + Conncected Links");

		ScriptProxy::GetCommandTracker(myScript).DoCommand<FunctionCommand<DestroyNodeData>>(CommandContext{ &myScript, myCurrentNodeGraph }, data,
			[](const DestroyNodeData& aData, const CommandContext& aContext) -> void
			{
				Node& node = ScriptProxy::GetNodeRef(*aContext.nodeGraph, aData.nodeID);
				node.isDestroyed = true;
				ScriptProxy::GetNodeExecutor(*aContext.script).UnbindFromEvent(NodeRef{ aData.nodeID, aContext.nodeGraph });

			},
			[](const DestroyNodeData& aData, const CommandContext& aContext) -> void
			{
				Node& node = ScriptProxy::GetNodeRef(*aContext.nodeGraph, aData.nodeID);
				node.isDestroyed = false;

				ScriptProxy::GetNodeExecutor(*aContext.script).BindToEvent(NodeRef{ aData.nodeID, aContext.nodeGraph });
			}, "Destroy Node"
		);

		InternalModifier::UnbindVariable(myScript, aNodeID);

		for (PinID inputPinID : ScriptProxy::GetNode(*myCurrentNodeGraph, aNodeID).inputPins)
		{
			DestroyLink(inputPinID);
		}

		for (PinID outputPinID : ScriptProxy::GetNode(*myCurrentNodeGraph, aNodeID).outputPins)
		{
			const Pin& outputPin = ScriptProxy::GetPin(*myCurrentNodeGraph, outputPinID);

			for (PinID connectedInputPin : outputPin.connectedPinIDs)
			{
				DestroyLink(connectedInputPin);

			}
		}


		ScriptProxy::GetCommandTracker(myScript).EndComposite();
	}

	void ScriptModifier::DestroySelection(const std::vector<NodeID>& aNodeIDs, const std::vector<LinkID>& aLinkIDs)
	{
		ScriptProxy::GetCommandTracker(myScript).BeginComposite("Destroy Selection");

		for (LinkID linkID : aLinkIDs)
		{
			DestroyLink(linkID);
		}

		for (NodeID nodeID : aNodeIDs)
		{
			DestroyNode(nodeID);
		}

		ScriptProxy::GetCommandTracker(myScript).EndComposite();
	}

	void ScriptModifier::SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, bool aTrackChange)
	{
		SetNodePosition(aNodeID, aPosition, ScriptProxy::GetNode(*myCurrentNodeGraph, aNodeID).position, aTrackChange);

	}
	void ScriptModifier::SetNodePosition(const NodeID aNodeID, ScriptVec2 aPosition, ScriptVec2 aOldPosition, bool aTrackChange)
	{
		struct SetNodePositionData
		{
			NodeID nodeID = InvalidID<NodeID>();
			ScriptVec2 oldPos;
			ScriptVec2 newPos;
		} data;

		if (aPosition != aOldPosition)
		{
			CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
			bool prevTracking = commandTracker.IsTracking();
			commandTracker.IsTracking() = aTrackChange;

			data.nodeID = aNodeID;
			data.oldPos = aOldPosition;
			data.newPos = aPosition;

			commandTracker.DoCommand<FunctionCommand<SetNodePositionData>>(CommandContext{ &myScript, myCurrentNodeGraph }, data,
				[](const SetNodePositionData& aData, const CommandContext& aContext) -> void
				{
					Node& node = ScriptProxy::GetNodeRef(*aContext.nodeGraph, aData.nodeID);
					node.position = aData.newPos;
				},
				[](const SetNodePositionData& aData, const CommandContext& aContext) -> void
				{
					Node& node = ScriptProxy::GetNodeRef(*aContext.nodeGraph, aData.nodeID);
					node.position = aData.oldPos;
				}, "Set Node Position"
			);


			commandTracker.IsTracking() = prevTracking;
		}

	}

	void ScriptModifier::BeginNodeDrag(const std::vector<NodeDragData>& aDragData)
	{
		for (const NodeDragData& dragData : aDragData)
		{
			BeginNodeDrag(dragData.nodeID, dragData.pos);
		}
	}

	void ScriptModifier::EndNodeDrag(const std::vector<NodeDragData>& aDragData)
	{
		CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);

		commandTracker.BeginComposite("Move Nodes");
		for (const NodeDragData& dragData : aDragData)
		{
			EndNodeDrag(dragData.nodeID, dragData.pos);
		}
		commandTracker.EndComposite();
	}

	void ScriptModifier::BeginNodeDrag(const NodeID aNodeID, ScriptVec2 aStartPos)
	{
		MoveNodeData& data = myMoveNodesData[aNodeID];
		data.startPos = aStartPos;
	}

	void ScriptModifier::EndNodeDrag(const NodeID aNodeID, ScriptVec2 aEndPos)
	{
		if (myMoveNodesData.contains(aNodeID))
		{
			MoveNodeData& data = myMoveNodesData.at(aNodeID);
			data.endPos = aEndPos;

			SetNodePosition(aNodeID, data.endPos, data.startPos, true);

			myMoveNodesData.erase(aNodeID);
		}

	}

	VarID ScriptModifier::CreateVariable(DataTypeID aDataTypeID)
	{
		std::vector<Variable>& variables = ScriptProxy::GetVariablesRef(myScript);
		VarID id = variables.size();
		variables.emplace_back();
		myScript.GetModifier().SetVariableDataType(id, aDataTypeID);
		return id;

	}

	void ScriptModifier::DestroyVariable(VarID aVarID)
	{
		CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
		commandTracker.BeginComposite("Destroy Variable + Connected Nodes");

		struct DestroyVariableData
		{
			VarID varID = InvalidID<VarID>();
		} data;

		data.varID = aVarID;

		commandTracker.DoCommand<FunctionCommand<DestroyVariableData>>(CommandContext{ &myScript }, data,
			[](const DestroyVariableData& aData, const CommandContext& aContext) -> void
			{
				ScriptProxy::GetVariableRef(*aContext.script, aData.varID).isDestroyed = true;
			},
			[](const DestroyVariableData& aData, const CommandContext& aContext) -> void
			{
				ScriptProxy::GetVariableRef(*aContext.script, aData.varID).isDestroyed = false;
			}
		);

		const VariableManager& variableManager = ScriptProxy::GetVariableManager(myScript);
		DestroySelection(variableManager.GetNodeIDsByVarID(aVarID), {});

		commandTracker.EndComposite();

	}

	void ScriptModifier::EditPin(PinID aPinID)
	{
		const Pin& pin = ScriptProxy::GetPin(*myCurrentNodeGraph, aPinID);
		const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);


		DataTypeManager::EditData(pinType.dataTypeID, pin.dataPtr);

		//MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(*myCurrentNodeGraph);

		/*DataTypeID dataTypeID = pinType.dataTypeID;
		void* dataPtr = pin.dataPtr;

		MemoryPool tempPool(64);
		MemoryPoolID tempID = DataTypeManager::AllocateData(dataTypeID, tempPool, dataPtr);
		void* copyDataPtr = tempPool.MemoryAt(tempID);

		if (DataTypeManager::EditData(dataTypeID, dataPtr))
		{

			MemoryPoolID prevMemoryID = DataTypeManager::AllocateData(dataTypeID, memoryPool, copyDataPtr);

			CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);

			struct EditValueData
			{
				DataTypeID dataTypeID = InvalidID<DataTypeID>();
				MemoryPoolID previousMemoryID = InvalidID<MemoryPoolID>();
				MemoryPoolID actualMemoryID = InvalidID<MemoryPoolID>();
			} data;

			data.dataTypeID = dataTypeID;
			data.previousMemoryID = prevMemoryID;
			data.actualMemoryID = pin.memoryID;

			commandTracker.RegisterCommand<FunctionCommand<EditValueData>>(CommandContext{ myScript, myCurrentNodeGraph }, data,
				[](const EditValueData& aData, const CommandContext& aContext) -> void
				{
					MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(*aContext.nodeGraph);

					DataTypeID dataTypeID = aData.dataTypeID;
					void* oldDataPtr = memoryPool.MemoryAt(aData.previousMemoryID);
					void* newDataPtr = memoryPool.MemoryAt(aData.actualMemoryID);

					DataTypeManager::SwapData(dataTypeID, oldDataPtr, newDataPtr);
				},
				[](const EditValueData& aData, const CommandContext& aContext) -> void
				{
					MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(*aContext.nodeGraph);

					DataTypeID dataTypeID = aData.dataTypeID;
					void* oldDataPtr = memoryPool.MemoryAt(aData.previousMemoryID);
					void* newDataPtr = memoryPool.MemoryAt(aData.actualMemoryID);

					DataTypeManager::SwapData(dataTypeID, newDataPtr, oldDataPtr);
				}, "Edit Pin Data"
			);
		}*/
	}

	void ScriptModifier::EditVariableDefaultValue(VarID aVarID)
	{
		Variable& variable = ScriptProxy::GetVariableRef(myScript, aVarID);

		//MemoryPoolID id = variable.defaultValueMemoryID;

		//MemoryManager& memoryManager = ScriptProxy::GetVariableMemoryManager(myScript);

		void* dataPtr = variable.runtimeDataPtr;
		if (DataTypeManager::EditData(variable.dataTypeID, dataPtr))
		{
			DataTypeManager::CopyData(variable.dataTypeID, variable.runtimeDataPtr, dataPtr);
		}
	}

	void ScriptModifier::SetVariableDataType(VarID aVarID, DataTypeID aDataTypeID)
	{
		Variable& variable = ScriptProxy::GetVariableRef(myScript, aVarID);

		void* runtimeDataPtr = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryManager(myScript));
		void* defaultValueDataPtr = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryManager(myScript));
		//MemoryPoolID runtimeMemID = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryPool(myScript));
		//MemoryPoolID defaultValueMemID = DataTypeManager::AllocateData(aDataTypeID, ScriptProxy::GetVariableMemoryPool(myScript));

		variable.dataTypeID = aDataTypeID;
		//variable.runtimeMemoryID = runtimeMemID;
		//variable.defaultValueMemoryID = defaultValueMemID;
		variable.runtimeDataPtr = runtimeDataPtr;
		variable.defaultValueDataPtr = defaultValueDataPtr;

		DestroyVariableNodes(aVarID);
	}

	void ScriptModifier::SetVariableName(VarID aVarID, const std::string& aName)
	{
		Variable& variable = ScriptProxy::GetVariableRef(myScript, aVarID);

		variable.name = aName;
	}

	void ScriptModifier::DestroyVariableNodes(const VarID aVarID)
	{
		DestroySelection(ScriptProxy::GetVariableManager(myScript).GetNodeIDsByVarID(aVarID), {});
	}

	void ScriptModifier::CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs)
	{
		if (aNodeIDs.empty())
		{
			return;
		}

		//CopyBuffer& copyBuffer = ScriptProxy::GetGlobalCopyBufferRef(myScript);

		//copyBuffer = CopyBuffer{};

		////CreateNode(0);

		//ScriptVec2 nodeTotalPos;
		//for (NodeID nodeID : aNodeIDs)
		//{
		//	const Node& node = ScriptProxy::GetNode(*myCurrentNodeGraph, nodeID);
		//	nodeTotalPos += node.position;


		//	NodeCopy& nodeCopy = copyBuffer.nodes.emplace_back();
		//	nodeCopy.typeID = node.typeID;

		//	//const MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(*myCurrentNodeGraph);

		//	for (PinID inputPinID : node.inputPins)
		//	{
		//		PinCopy& pinCopy = nodeCopy.inputPinCopies.emplace_back();

		//		const Pin& inputPin = ScriptProxy::GetPin(*myCurrentNodeGraph, inputPinID);
		//		const PinType& inputPinType = PinTypeManager::GetPinType(inputPin.typeID);


		//		//const void* sourceDataPtr = memoryPool.MemoryAt(inputPin.memoryID);
		//		const void* sourceDataPtr = inputPin.dataPtr;
		//		pinCopy.memoryID = DataTypeManager::AllocateData(inputPinType.dataTypeID, nodeCopy.data, sourceDataPtr);
		//	}

		//	for (PinID outputPinID : node.outputPins)
		//	{
		//		PinCopy& pinCopy = nodeCopy.outputPinCopies.emplace_back();

		//		const Pin& outputPin = ScriptProxy::GetPin(*myCurrentNodeGraph, outputPinID);
		//		const PinType& outputPinType = PinTypeManager::GetPinType(outputPin.typeID);

		//		//const void* sourceDataPtr = memoryPool.MemoryAt(outputPin.memoryID); 
		//		const void* sourceDataPtr = outputPin.dataPtr;
		//		pinCopy.memoryID = DataTypeManager::AllocateData(outputPinType.dataTypeID, nodeCopy.data, sourceDataPtr);
		//	}

		//}

		//ScriptVec2 avgPos = nodeTotalPos / static_cast<float>(aNodeIDs.size());

		//// Calculate difference from avg pos for every copied node

		//for (size_t i = 0; i < aNodeIDs.size(); ++i)
		//{
		//	const Node& node = ScriptProxy::GetNode(*myCurrentNodeGraph, aNodeIDs[i]);

		//	NodeCopy& nodeCopy = copyBuffer.nodes[i];
		//	nodeCopy.diffFromAvg = node.position - avgPos;
		//}


	}

	void ScriptModifier::PasteCopyBuffer(ScriptVec2 aPosition)
	{
		aPosition;
		//const CopyBuffer& copyBuffer = ScriptProxy::GetGlobalCopyBufferRef(myScript);
		//if (copyBuffer.nodes.empty())
		//{
		//	return;
		//}

		//CommandTracker& commandTracker = ScriptProxy::GetCommandTracker(myScript);
		//commandTracker.BeginComposite(myScript, "Paste Nodes");


		//MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(*myCurrentNodeGraph);

		//// Create nodes
		//for (const NodeCopy& nodeCopy : copyBuffer.nodes)
		//{
		//	ScriptVec2 newPos = aPosition + nodeCopy.diffFromAvg;
		//	NodeID createdNodeID = CreateNode(nodeCopy.typeID, newPos);

		//	const Node& createdNode = ScriptProxy::GetNode(*myCurrentNodeGraph, createdNodeID);


		//	assert(createdNode.inputPins.size() == nodeCopy.inputPinCopies.size());
		//	assert(createdNode.outputPins.size() == nodeCopy.outputPinCopies.size());

		//	for (size_t i = 0; i < createdNode.inputPins.size(); ++i)
		//	{
		//		const Pin& inputPin = ScriptProxy::GetPin(*myCurrentNodeGraph, createdNode.inputPins[i]);
		//		const PinType& inputPinType = PinTypeManager::GetPinType(inputPin.typeID);

		//		//void* dataPtr = memoryPool.MemoryAt(inputPin.memoryID);
		//		const void* sourceDataPtr = nodeCopy.data.MemoryAt(nodeCopy.inputPinCopies[i].memoryID);

		//		DataTypeManager::CopyData(inputPinType.dataTypeID, inputPin.dataPtr, sourceDataPtr);

		//	}

		//	for (size_t i = 0; i < createdNode.outputPins.size(); ++i)
		//	{
		//		const Pin& outputPin = ScriptProxy::GetPin(*myCurrentNodeGraph, createdNode.outputPins[i]);
		//		const PinType& outputPinType = PinTypeManager::GetPinType(outputPin.typeID);

		//		//void* dataPtr = memoryPool.MemoryAt(outputPin.memoryID);
		//		const void* sourceDataPtr = nodeCopy.data.MemoryAt(nodeCopy.outputPinCopies[i].memoryID);

		//		DataTypeManager::CopyData(outputPinType.dataTypeID, outputPin.dataPtr, sourceDataPtr);

		//	}
		//}

		// Create links
		/*for (const auto& [copiedNodeID, pastedNodeID] : copyMap)
		{
			const Node& copyNode = ScriptProxy::GetNode(myScript, copiedNodeID);
			const Node& pasteNode = ScriptProxy::GetNode(myScript, pastedNodeID);

			for (size_t i = 0; i < copyNode.inputPins.size(); ++i)
			{
				const Pin& copyInputPin = ScriptProxy::GetPin(myScript, copyNode.inputPins[i]);

				const PinID pasteInputPinID = pasteNode.inputPins[i];

				for (size_t j = 0; j < copyInputPin.connectedPinIDs.size(); ++j)
				{

					PinID copyConnectedPinID = copyInputPin.connectedPinIDs[j];
					const Pin& copyConnectedPin = ScriptProxy::GetPin(myScript, copyConnectedPinID);

					if (copyMap.contains(copyConnectedPin.nodeID))
					{
						NodeID newConnectedNodeID = copyMap.at(copyConnectedPin.nodeID);
						const Node& newConnectedNode = ScriptProxy::GetNode(myScript, newConnectedNodeID);

						PinID newConnectedPinID = newConnectedNode.outputPins[ScriptLinker::GetPinIndex(myScript, copyConnectedPinID, ePinFlowType::Output)];

						TryCreateLink(pasteInputPinID, newConnectedPinID);
					}
				}
			}
		}*/

		//commandTracker.EndComposite();

	}

	CustomEventID ScriptModifier::CreateNodeType_CustomEvent(const std::string& aName)
	{
		return InternalModifier::CreateCustomEvent(aName);
	}

	void ScriptModifier::AddPinToCustomEvent(const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, const std::string& aName, ScriptFoundation* aFoundation)
	{
		CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);


		NodeType& nodeType = NodeTypeManager::GetNodeType(customEvent.GetExecutorTypeID());

		PinTypeID executorPinTypeID = PinTypeManager::Create(aName, ePinFlowType::Output, aDataTypeID, CreatePinSetFunction());
		nodeType.nodeRecipe.outputPinTypeIDs.push_back(executorPinTypeID);


		NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.GetCallerTypeID());
		PinTypeID callerPinTypeID = PinTypeManager::Create(aName, ePinFlowType::Input, aDataTypeID, CreatePinSetFunction());
		callerNodeType.nodeRecipe.inputPinTypeIDs.push_back(callerPinTypeID);

		if (!aFoundation)
		{
			return;
		}

		for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*aFoundation))
		{
			for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
			{
				// TODO - fix
				NodeGraph& eventGraph = ScriptProxy::GetEventGraph(*script);

				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetExecutorTypeID()))
				{
					PinID pinID = InternalModifier::CreateOutputPin(eventGraph, nodeID, executorPinTypeID);

					ScriptProxy::GetNodeRef(eventGraph, nodeID).outputPins.push_back(pinID);
				}

				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetCallerTypeID()))
				{
					PinID pinID = InternalModifier::CreateInputPin(eventGraph, nodeID, callerPinTypeID);

					ScriptProxy::GetNodeRef(eventGraph, nodeID).inputPins.push_back(pinID);
				}
			}
		}

	}

	void ScriptModifier::SetPinAtIndexCustomEvent(const size_t anIndex, const DataTypeID aDataTypeID, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation)
	{
		if (anIndex == 0)
		{
			return;
		}
		CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);

		NodeType& nodeType = NodeTypeManager::GetNodeType(customEvent.GetExecutorTypeID());

		if (anIndex >= nodeType.nodeRecipe.outputPinTypeIDs.size())
		{
			return;
		}

		const PinType& oldExecutorPinType = PinTypeManager::GetPinType(nodeType.nodeRecipe.outputPinTypeIDs[anIndex]);
		if (oldExecutorPinType.dataTypeID == aDataTypeID)
		{
			return;
		}

		PinTypeID newExecutorPinTypeID = PinTypeManager::Create(oldExecutorPinType.name, ePinFlowType::Output, aDataTypeID, CreatePinSetFunction());
		nodeType.nodeRecipe.outputPinTypeIDs[anIndex] = newExecutorPinTypeID;



		NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.GetCallerTypeID());

		const PinType& oldCallerPinType = PinTypeManager::GetPinType(callerNodeType.nodeRecipe.inputPinTypeIDs[anIndex]);

		PinTypeID newCallerPinTypeID = PinTypeManager::Create(oldCallerPinType.name, ePinFlowType::Input, aDataTypeID, CreatePinSetFunction());
		callerNodeType.nodeRecipe.inputPinTypeIDs[anIndex] = newCallerPinTypeID;

		if (!aFoundation)
		{
			return;
		}

		for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*aFoundation))
		{
			for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
			{
				// TODO - fix
				NodeGraph& eventGraph = ScriptProxy::GetEventGraph(*script);

				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetExecutorTypeID()))
				{
					PinID pinID = InternalModifier::CreateOutputPin(eventGraph, nodeID, newExecutorPinTypeID);

					ScriptProxy::GetNodeRef(eventGraph, nodeID).outputPins[anIndex] = pinID;

				}

				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetCallerTypeID()))
				{
					PinID pinID = InternalModifier::CreateInputPin(eventGraph, nodeID, newCallerPinTypeID);

					ScriptProxy::GetNodeRef(eventGraph, nodeID).inputPins[anIndex] = pinID;
				}
			}
		}

	}

	void ScriptModifier::DeletePinAtIndexCustomEvent(const size_t anIndex, const CustomEventID aNodeTypeID, ScriptFoundation* aFoundation)
	{
		if (anIndex == 0)
		{
			return;
		}

		CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(aNodeTypeID);

		NodeType& executorNodeType = NodeTypeManager::GetNodeType(customEvent.GetExecutorTypeID());

		if (anIndex >= executorNodeType.nodeRecipe.outputPinTypeIDs.size())
		{
			return;
		}

		executorNodeType.nodeRecipe.outputPinTypeIDs.erase(executorNodeType.nodeRecipe.outputPinTypeIDs.begin() + anIndex);


		NodeType& callerNodeType = NodeTypeManager::GetNodeType(customEvent.GetCallerTypeID());
		callerNodeType.nodeRecipe.inputPinTypeIDs.erase(callerNodeType.nodeRecipe.inputPinTypeIDs.begin() + anIndex);

		if (!aFoundation)
		{
			return;
		}

		for (const std::unique_ptr<ScriptManager>& scriptManager : ScriptProxy::GetScriptManagers(*aFoundation))
		{
			for (const std::unique_ptr<Script>& script : scriptManager->GetScripts())
			{
				// TODO
				NodeGraph& eventGraph = ScriptProxy::GetEventGraph(*script);
				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetExecutorTypeID()))
				{

					Node& node = ScriptProxy::GetNodeRef(eventGraph, nodeID);
					script->GetModifier().DestoryLinksByOutputPinID(node.outputPins[anIndex]);

					node.outputPins.erase(node.outputPins.begin() + anIndex);
				}


				for (NodeID nodeID : ScriptProxy::GetNodeIDsByNodeType(eventGraph, customEvent.GetCallerTypeID()))
				{
					Node& node = ScriptProxy::GetNodeRef(eventGraph, nodeID);

					script->GetModifier().DestroyLink(node.inputPins[anIndex]);
					node.inputPins.erase(node.inputPins.begin() + anIndex);
				}
			}
		}
	}

	FunctionID ScriptModifier::CreateFunction(const std::string& aName)
	{
		return InternalModifier::CreateFunction(aName);
	}

	void ScriptModifier::SetCurrentNodeGraph(NodeGraph* aNodeGraph)
	{
		myCurrentNodeGraph = aNodeGraph;
	}

	NodeGraph* ScriptModifier::GetCurrentNodeGraph() const
	{
		return myCurrentNodeGraph;
	}

}

