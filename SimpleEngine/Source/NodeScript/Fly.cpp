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

	GenericDataTypeProxy CreateStruct(std::string_view aName, std::string_view aSavePath)
	{
		const DataTypeID dataTypeID = Internal::CreateStruct(aName);
		Internal::SaveDataType(*Internal::GetDataTypeByID(dataTypeID), aSavePath);
		return GenericDataTypeProxy(dataTypeID);
	}

	ClassProxy CreateClass(const GenericDataTypeProxy aTargetProxy, const std::string_view aName, const std::string_view aSavePath)
	{
		ClassID createdClassID = Internal::CreateClass(aTargetProxy.GetID(), aName);
		Internal::SaveClass(Internal::GetClassByID(createdClassID), aSavePath);
		return ClassProxy(createdClassID);
	}

	ClassProxy CreateClassWithoutTarget(const std::string_view aName, const std::string_view aSavePath)
	{
		return CreateClass(GenericDataTypeProxy(GetDataTypeID<None*>()), aName, aSavePath);
	}

	GenericDataTypeProxy FindDataTypeByName(const std::string_view aName)
	{
		return GenericDataTypeProxy{ Internal::GetDataTypeManager().GetDataTypeIDByName(aName) };
	}

	ClassProxy FindClassByName(const std::string_view aName)
	{
		auto& classes = Internal::GetDataTypeManager().GetClasses();

		for (size_t i = 0; i < classes.size(); ++i)
		{
			if (classes[i]->mName == aName)
			{
				return ClassProxy(ClassID{ i });
			}
		}

		return ClassProxy();
	}



	void SetDefaultDataTypeColor(const Fly::Color& aColor)
	{
		Internal::GetDataTypeManager().SetDefaultColor(aColor);
	}

	void SetEditorTextFunction(void(*aTextFunction)(const std::string&))
	{
		Internal::GetDataTypeManager().SetEditorTextFunction(aTextFunction);
	}

	void CreateCopyBuffer(const std::vector<NodeID>& aNodeIDs, const NodeGraphProxy aCopiedFromNodeGraphProxy)
	{
		if (aNodeIDs.empty())
		{
			return;
		}
		const NodeGraph& copiedFromNodeGraph = aCopiedFromNodeGraphProxy.GetNodeGraph();
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

				Internal::GetDataTypeManager().CopyData(copiedInputPinType.mGenericDataTypeID, createdInputPin.mDataPtr, copiedInputPin.mDataPtr);
			}
		}
	}

	void PasteCopyBuffer(const Vec2 aPosition, NodeGraphProxy aTargetNodeGraphProxy, CommandTracker* const aCommandTracker)
	{
		const NodeGraph& nodeGraphCopy = Internal::GetNodeGraphCopy().mNodeGraph;

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Paste Nodes");
		}

		NodeGraph& targetNodeGraph = aTargetNodeGraphProxy.GetNodeGraph();

		std::unordered_map<NodeID, NodeID> nodeConverter;

		for (NodeID sourceNodeID{ 0 }; sourceNodeID < nodeGraphCopy.mNodes.size(); sourceNodeID++)
		{
			const Node& node = nodeGraphCopy.mNodes.at(sourceNodeID);
			const NodeID createdNodeID = Internal::CreateNode(aTargetNodeGraphProxy.GetVariant(), node.mTypeID, aPosition + node.mPosition, aCommandTracker);
			nodeConverter.emplace(sourceNodeID, createdNodeID);

			const Node& createdNode = Internal::GetNode(createdNodeID, targetNodeGraph);
			DataTypeManager& dataTypeManager = Internal::GetDataTypeManager();
			for (const PinID createdInputPinID : createdNode.mInputPins)
			{
				Pin& createdInputPin = Internal::GetPin(createdInputPinID, targetNodeGraph);
				const PinType& createdInputPinType = Internal::GetPinTypeManager().GetPinType(createdInputPin.mTypeID);

				const PinID sourcePinID = Internal::GetOpposingPinID(targetNodeGraph, createdInputPinID, nodeGraphCopy, sourceNodeID);
				const Pin& sourcePin = nodeGraphCopy.mPins.at(sourcePinID);
				dataTypeManager.CopyData(createdInputPinType.mGenericDataTypeID, createdInputPin.mDataPtr, sourcePin.mDataPtr);
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

	CustomEventProxy CreateCustomEvent(const std::string_view aName)
	{
		return CustomEventProxy(Internal::CreateCustomEvent(aName));
	}

	FunctionProxy CreateGlobalFunction(const std::string_view aName)
	{
		return FunctionProxy(Internal::CreateFunction(aName));
	}

	void BeginFrame(CommandTracker* const aCommandTracker)
	{
		Internal::BeginFrame(aCommandTracker);
	}

	template<IsPredicate<const Pin&> Predicate>
	std::vector<PinProxy> GetPinProxysFiltered(Predicate&& aPredicate, const NodeGraphProxy& aNodeGraphProxy)
	{
		std::vector<PinProxy> pinProxys;
		const NodeGraph& nodeGraph = aNodeGraphProxy.GetNodeGraph();
		pinProxys.reserve(nodeGraph.mPins.size());

		for (PinID pinID = 0; pinID < nodeGraph.mPins.size(); ++pinID)
		{
			const Pin& pin = nodeGraph.mPins[pinID];
			if (aPredicate(pin))
			{
				pinProxys.push_back(PinProxy(pinID, aNodeGraphProxy));
			}
		}

		return pinProxys;
	}

	bool& IsDebugging()
	{
		return Internal::IsDebugging();
	}

	std::vector<DataTypeProxy> GetDataTypes()
	{
		const auto& dataTypes = Internal::GetDataTypeManager().GetDataTypes();
		std::vector<DataTypeProxy> dataTypeProxys;
		dataTypeProxys.reserve(dataTypes.size());

		for (const auto& [dataTypeID, dataType] : dataTypes)
		{
			dataTypeProxys.push_back(DataTypeProxy(dataTypeID));
		}

		return dataTypeProxys;
	}

	std::vector<GenericDataTypeProxy> GetGenericDataTypes()
	{
		const auto& dataTypes = Internal::GetDataTypeManager().GetDataTypes();
		std::vector<GenericDataTypeProxy> dataTypeProxys;
		dataTypeProxys.reserve(dataTypes.size());

		for (const auto& [dataTypeID, dataType] : dataTypes)
		{
			dataTypeProxys.push_back(GenericDataTypeProxy(dataTypeID));
		}

		return dataTypeProxys;
	}

	DataTypeProxy GetDataTypeProxyByName(std::string_view aName)
	{
		return DataTypeProxy(Internal::GetDataTypeManager().GetDataTypeIDByName(std::string(aName)));
	}

	std::vector<NodeTypeProxy> GetNodeTypes()
	{
		const auto& nodeTypes = Internal::GetNodeTypeManager().GetNodeTypes();
		std::vector<NodeTypeProxy> views;
		views.reserve(nodeTypes.size());

		for (NodeTypeID nodeTypeID{ 0 }; nodeTypeID < nodeTypes.size(); ++nodeTypeID)
		{
			views.push_back(NodeTypeProxy(nodeTypeID));
		}

		return views;
	}

	std::vector<FunctionProxy> GetFunctions()
	{
		const auto& mFunctions = Internal::GetNodeTypeManager().GetFunctions();
		std::vector<FunctionProxy> views;
		views.reserve(mFunctions.size());

		for (FunctionID functionID{ 0 }; functionID < mFunctions.size(); ++functionID)
		{
			views.push_back(FunctionProxy(functionID));
		}

		return views;
	}

	std::vector<CustomEventProxy> GetCustomEvents()
	{
		const auto& customEvents = Internal::GetNodeTypeManager().GetCustomEvents();
		std::vector<CustomEventProxy> customEventProxys;
		customEventProxys.reserve(customEvents.size());

		for (CustomEventID customEventID{ 0 }; customEventID < customEvents.size(); ++customEventID)
		{
			customEventProxys.push_back(CustomEventProxy(customEventID));
		}

		return customEventProxys;
	}

	std::vector<LinkProxy> GetTraversedLinks()
	{
		std::vector<LinkProxy> linkProxys;
		const std::vector<LinkRef> linkRefs = Internal::GetNodeExecutor().GetDebugger().GetTraversedLinks();
		linkProxys.reserve(linkRefs.size());

		for (auto& linkRef : linkRefs)
		{
			linkProxys.push_back(LinkProxy(linkRef.mLinkID, NodeGraphProxy(linkRef.mNodeGraphVariantHandle)));
		}

		return linkProxys;
	}

	template<typename FilterFunction>
	std::vector<NodeTypeProxy> GetNodeTypesFiltered(FilterFunction&& aFilter)
	{
		std::vector<NodeTypeProxy> facades;
		const std::vector<NodeType>& nodeTypes = Internal::GetNodeTypeManager().GetNodeTypes();
		facades.reserve(nodeTypes.size());
		for (NodeTypeID id{ 0 }; id < nodeTypes.size(); id++)
		{
			if (aFilter(nodeTypes[id]))
			{
				facades.push_back(NodeTypeProxy(id));
			}
		}
		return facades;
	}

	std::vector<NodeTypeProxy> GetNodeTypesFilteredByDataTypeAndFlowType(const GenericDataTypeID aDataTypeID, const eFlowType aFlowType)
	{
		return GetNodeTypesFiltered([aDataTypeID, aFlowType](const NodeType& aNodeType) -> bool
			{
				const std::vector<PinTypeID>& pinTypeIDs = SelectByFlowType(aFlowType, aNodeType.mNodeRecipe.mInputPinTypeIDs, aNodeType.mNodeRecipe.mOutputPinTypeIDs);
				for (const PinTypeID pinTypeID : pinTypeIDs)
				{
					const PinType& pinType = Internal::GetPinTypeManager().GetPinType(pinTypeID);
					if (pinType.mGenericDataTypeID == aDataTypeID)
					{
						return true;
					}
				}
				return false;
			}
		);
	}

	NodeTypeProxyIteratorService IterateNodeTypes()
	{
		return NodeTypeProxyIteratorService(NodeTypeID{ Internal::GetNodeTypeManager().GetNodeTypes().size() });
	}

	NodeTypeProxyIteratorService IterateNodeTypes(Predicate<NodeTypeProxy> aFilterPredicate)
	{
		return NodeTypeProxyIteratorService(NodeTypeID{ Internal::GetNodeTypeManager().GetNodeTypes().size() }, aFilterPredicate);
	}

	std::vector<NodeTypeProxy> GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(const GenericDataTypeID aDataTypeID, const eFlowType aFlowType, const eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
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
					const GenericDataTypeID inputDataTypeID = SelectByFlowType(aFlowType, pinType.mGenericDataTypeID, aDataTypeID);
					const GenericDataTypeID outputDataTypeID = SelectByFlowType(aFlowType, aDataTypeID, pinType.mGenericDataTypeID);
					if (Internal::AreDataTypesLinkable(inputDataTypeID, outputDataTypeID))
					{
						return true;
					}
				}
				return false;
			}
		);
	}

	std::vector<NodeTypeProxy> GetNodeTypesFilteredByTrait(const eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
	{
		return GetNodeTypesFiltered([aNodeTrait, aBitOperation](const NodeType& aNodeType) -> bool
			{
				return aBitOperation(aNodeTrait, aNodeType.mNodeRecipe.mTraits);
			}
		);
	}

	std::unordered_map<DataTypeProxy, std::vector<ClassProxy>> GetClasses()
	{
		auto& classes = Internal::GetDataTypeManager().GetClasses();

		std::unordered_map<DataTypeProxy, std::vector<ClassProxy>> views;

		for (size_t i = 0; i < classes.size(); i++)
		{
			views[DataTypeProxy(classes[i]->mTargetID)].push_back(ClassProxy(ClassID{ i }));
		}

		return views;
	}

	std::vector<ClassProxy> GetClassesByTargetDataType(const DataTypeProxy aDataTypeProxy)
	{
		auto& classes = Internal::GetDataTypeManager().GetClasses();

		std::vector<ClassProxy> views;

		for (size_t i = 0; i < classes.size(); ++i)
		{
			if (classes[i]->mTargetID == aDataTypeProxy.GetID())
			{
				views.push_back(ClassProxy(ClassID{ i }));
			}
		}

		return views;
	}
}

