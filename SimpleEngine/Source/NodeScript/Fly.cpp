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

	void SetDefaultDataTypeColor(const Fly::Color& aColor)
	{
		Global::GetDataTypeManager().SetDefaultColor(aColor);
	}

	void SetEditorNullptrFunction(void(*aFunction)())
	{
		Global::GetDataTypeManager().SetEditorNullptrFunction(aFunction);
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

	FunctionFacade CreateGlobalFunction(const std::string_view aName)
	{
		return FunctionFacade(Internal::CreateFunction(aName));
	}

	void BeginFrame(CommandTracker* const aCommandTracker)
	{
		Internal::BeginFrame(aCommandTracker);
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

	std::vector<LinkFacade> GetTraversedLinks()
	{
		std::vector<LinkFacade> linkFacades;
		const std::vector<LinkRef> linkRefs = Global::GetNodeExecutor().GetDebugger().GetTraversedLinks();
		linkFacades.reserve(linkRefs.size());

		for (auto& linkRef : linkRefs)
		{
			linkFacades.push_back(LinkFacade(linkRef.mLinkID, NodeGraphFacade(linkRef.mNodeGraphVariantHandle)));
		}

		return linkFacades;
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

	std::vector<NodeTypeFacade> GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(const DataTypeID aDataTypeID, const eFlowType aFlowType, eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
	{
		return GetNodeTypesFiltered([aDataTypeID, aFlowType, aNodeTrait, aBitOperation](const NodeType& aNodeType) -> bool
			{
				if (!aBitOperation(aNodeTrait, aNodeType.mNodeRecipe.mTraits))
				{
					return false;
				}
				const std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, aNodeType.mNodeRecipe.mInputPinTypeIDs, aNodeType.mNodeRecipe.mOutputPinTypeIDs);
				for (const PinTypeID pinTypeID : pinTypeIDs)
				{
					const PinType& pinType = Global::GetPinTypeManager().GetPinType(pinTypeID);
					if (Global::GetDataTypeManager().AreDataTypesRelated(pinType.mDataTypeID, aDataTypeID))
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

