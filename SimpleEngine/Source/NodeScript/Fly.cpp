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

	void LoadAllFlyFiles(const std::string_view aFilePath)
	{
		Internal::LoadAllFlyFiles(aFilePath);
	}

	void SaveCustomEvents(const std::string_view aFilePath)
	{
		Internal::SaveCustomEvents(aFilePath);
	}

	StructFacade CreateStruct(std::string_view aName, std::string_view aSavePath)
	{
		StructID structID = Internal::CreateStruct(aName);
		Internal::SaveStruct(Internal::GetStructByID(structID), aSavePath);
		return StructFacade(structID);
	}

	ClassFacade CreateClass(const DataTypeFacade aTargetFacade, const std::string_view aName, const std::string_view aSavePath)
	{
		ClassID createdClassID = Internal::CreateClass(aTargetFacade.GetID(), aName);
		Internal::SaveClass(Internal::GetClassByID(createdClassID), aSavePath);
		return ClassFacade(createdClassID);
	}

	ClassFacade CreateClassWithoutTarget(const std::string_view aName, const std::string_view aSavePath)
	{
		return CreateClass(DataTypeFacade(GetDataTypeID<None*>()), aName, aSavePath);
	}


	StructFacade FindStructByName(const std::string_view aName)
	{
		return StructFacade{ Internal::GetDataTypeManager().GetStructIDByName(aName) };
	}

	ClassFacade FindClassByName(const std::string_view aName)
	{
		auto& classes = Internal::GetDataTypeManager().GetClasses();

		for (size_t i = 0; i < classes.size(); ++i)
		{
			if (classes[i]->mName == aName)
			{
				return ClassFacade(ClassID{ i });
			}
		}

		return ClassFacade();
	}



	void SetDefaultDataTypeColor(const Fly::Color& aColor)
	{
		Internal::GetDataTypeManager().SetDefaultColor(aColor);
	}

	void SetEditorNullptrFunction(void(*aFunction)())
	{
		Internal::GetDataTypeManager().SetEditorNullptrFunction(aFunction);
	}

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraphFacade aCopiedFromNodeGraphFacade)
	{
		if (aNodeIDs.empty())
		{
			return;
		}
		const NodeGraph& copiedFromNodeGraph = aCopiedFromNodeGraphFacade.GetNodeGraph();
		EventGraph& eventGraphCopy = Internal::GetNodeGraphCopy();
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
				const PinType& copiedInputPinType = Internal::GetPinTypeManager().GetPinType(copiedInputPin.mTypeID);

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

				Internal::GetDataTypeManager().CopyData(copiedInputPinType.mDataTypeID, createdInputPin.mDataPtr, copiedInputPin.mDataPtr);
			}
		}
	}

	void PasteCopyBuffer(const Vec2 aPosition, NodeGraphFacade aTargetNodeGraphFacade, CommandTracker* const aCommandTracker)
	{
		const NodeGraph& nodeGraphCopy = Internal::GetNodeGraphCopy().mNodeGraph;

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Paste Nodes");
		}

		NodeGraph& targetNodeGraph = aTargetNodeGraphFacade.GetNodeGraph();

		std::unordered_map<NodeID, NodeID> nodeConverter;

		for (NodeID sourceNodeID{ 0 }; sourceNodeID < nodeGraphCopy.mNodes.size(); sourceNodeID++)
		{
			const Node& node = nodeGraphCopy.mNodes.at(sourceNodeID);
			const NodeID createdNodeID = Internal::CreateNode(aTargetNodeGraphFacade.GetVariant(), node.mTypeID, aPosition + node.mPosition, aCommandTracker);
			nodeConverter.emplace(sourceNodeID, createdNodeID);

			const Node& createdNode = Internal::GetNode(createdNodeID, targetNodeGraph);
			DataTypeManager& dataTypeManager = Internal::GetDataTypeManager();
			for (const PinID createdInputPinID : createdNode.mInputPins)
			{
				Pin& createdInputPin = Internal::GetPin(createdInputPinID, targetNodeGraph);
				const PinType& createdInputPinType = Internal::GetPinTypeManager().GetPinType(createdInputPin.mTypeID);

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

	bool& IsDebugging()
	{
		return Internal::IsDebugging();
	}

	std::vector<DataTypeFacade> GetDataTypes()
	{
		const auto& dataTypes = Internal::GetDataTypeManager().GetDataTypes();
		std::vector<DataTypeFacade> dataTypeFacades;
		dataTypeFacades.reserve(dataTypes.size());

		for (const auto& [dataTypeID, dataType] : dataTypes)
		{
			dataTypeFacades.push_back(DataTypeFacade(dataTypeID));
		}

		return dataTypeFacades;
	}

	DataTypeFacade GetDataTypeFacadeByName(std::string_view aName)
	{
		return DataTypeFacade(Internal::GetDataTypeManager().GetDataTypeIDByName(std::string(aName)));
	}

	std::vector<NodeTypeFacade> GetNodeTypes()
	{
		const auto& nodeTypes = Internal::GetNodeTypeManager().GetNodeTypes();
		std::vector<NodeTypeFacade> views;
		views.reserve(nodeTypes.size());

		for (NodeTypeID nodeTypeID{ 0 }; nodeTypeID < nodeTypes.size(); ++nodeTypeID)
		{
			views.push_back(NodeTypeFacade(nodeTypeID));
		}

		return views;
	}

	std::vector<FunctionFacade> GetFunctions()
	{
		const auto& mFunctions = Internal::GetNodeTypeManager().GetFunctions();
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
		const auto& customEvents = Internal::GetNodeTypeManager().GetCustomEvents();
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
		const std::vector<LinkRef> linkRefs = Internal::GetNodeExecutor().GetDebugger().GetTraversedLinks();
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
		const std::vector<NodeType>& nodeTypes = Internal::GetNodeTypeManager().GetNodeTypes();
		facades.reserve(nodeTypes.size());
		for (NodeTypeID id{ 0 }; id < nodeTypes.size(); id++)
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
					const PinType& pinType = Internal::GetPinTypeManager().GetPinType(pinTypeID);
					if (pinType.mDataTypeID == aDataTypeID)
					{
						return true;
					}
				}
				return false;
			}
		);
	}

	std::vector<NodeTypeFacade> GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(const DataTypeID aDataTypeID, const eFlowType aFlowType, const eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
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
					const PinType& pinType = Internal::GetPinTypeManager().GetPinType(pinTypeID);
					const DataTypeID inputDataTypeID = SelectByFlowType(aFlowType, pinType.mDataTypeID, aDataTypeID);
					const DataTypeID outputDataTypeID = SelectByFlowType(aFlowType, aDataTypeID, pinType.mDataTypeID);
					if (Internal::AreDataTypesLinkable(inputDataTypeID, outputDataTypeID))
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
		auto& classes = Internal::GetDataTypeManager().GetClasses();

		std::unordered_map<DataTypeFacade, std::vector<ClassFacade>> views;

		for (size_t i = 0; i < classes.size(); i++)
		{
			views[DataTypeFacade(classes[i]->mTargetID)].push_back(ClassFacade(ClassID{ i }));
		}

		return views;
	}

	std::vector<ClassFacade> GetClassesByTargetDataType(const DataTypeFacade aDataTypeFacade)
	{
		auto& classes = Internal::GetDataTypeManager().GetClasses();

		std::vector<ClassFacade> views;

		for (size_t i = 0; i < classes.size(); ++i)
		{
			if (classes[i]->mTargetID == aDataTypeFacade.GetID())
			{
				views.push_back(ClassFacade(ClassID{ i }));
			}
		}

		return views;
	}
}

