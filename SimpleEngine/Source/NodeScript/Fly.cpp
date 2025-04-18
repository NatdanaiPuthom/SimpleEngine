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

	void CreateCopyBuffer(const std::span<NodeID> aNodeIDs, NodeGraphProxy aCopiedFromNodeGraphProxy)
	{
		if (aNodeIDs.empty())
		{
			return;
		}
		const NodeGraph& copiedFromNodeGraph = aCopiedFromNodeGraphProxy.GetNodeGraph();
		EventGraph& eventGraphCopy = Internal::GetNodeGraphCopy();
		eventGraphCopy = EventGraph{};

		NodeGraph& nodeGraphCopy = eventGraphCopy.GetNodeGraph();

		std::vector<NodeID> createdNodeIDs;
		std::unordered_map<NodeID, NodeID> nodeConverter;
		createdNodeIDs.reserve(aNodeIDs.size());

		Vec2 avgPos;
		for (const NodeID nodeID : aNodeIDs)
		{
			const Node& node = copiedFromNodeGraph.GetNode(nodeID);

			const NodeID createdNodeID = Internal::CreateNode(&eventGraphCopy, node.GetTypeID(), node.GetPosition(), nullptr);
			avgPos += node.GetPosition();

			createdNodeIDs.push_back(createdNodeID);
			nodeConverter.emplace(nodeID, createdNodeID);
		}

		avgPos /= static_cast<float>(aNodeIDs.size());

		for (NodeID nodeID{ 0 }; nodeID < nodeGraphCopy.GetNodeCount(); ++nodeID)
		{
			Node& node = nodeGraphCopy.GetNode(nodeID);
			node.SetPosition(node.GetPosition() - avgPos);
		}

		// Create links
		for (const NodeID copiedNodeID : aNodeIDs)
		{
			const Node& copiedNode = copiedFromNodeGraph.GetNode(copiedNodeID);
			for (const PinID copiedInputPinID : copiedNode.GetInputPins())
			{
				const Pin& copiedInputPin = Internal::GetPin(copiedInputPinID, copiedFromNodeGraph);
				const PinType& copiedInputPinType = Internal::GetPinTypeManager().GetPinType(copiedInputPin.GetTypeID());

				const NodeID createdNodeID = nodeConverter.at(copiedNodeID);
				const PinID createdInputPinID = Internal::GetOpposingPinID(copiedFromNodeGraph, copiedInputPinID, nodeGraphCopy, createdNodeID);
				assert(createdInputPinID != InvalidID<PinID>());

				const std::vector<LinkID> connectedLinks = Internal::GetLinkIDsByPin(copiedFromNodeGraph, copiedInputPinID);

				for (const LinkID connectedLinkID : connectedLinks)
				{
					const Link& connectedLink = copiedFromNodeGraph.GetLink(connectedLinkID);

					const PinID connectedOutputPinID = connectedLink.GetOutputPinID();
					const Pin& connectedOutputPin = Internal::GetPin(connectedOutputPinID, copiedFromNodeGraph);

					auto it = nodeConverter.find(connectedOutputPin.GetNodeID());
					if (it == end(nodeConverter))
					{
						continue;
					}
					const NodeID newConnectedNodeID = it->second;
					const PinID createdOutputPinID = Internal::GetOpposingPinID(copiedFromNodeGraph, connectedOutputPinID, nodeGraphCopy, newConnectedNodeID);

					Internal::TryCreateLink(nodeGraphCopy, createdInputPinID, createdOutputPinID, nullptr);
				}

				Pin& createdInputPin = Internal::GetPin(createdInputPinID, nodeGraphCopy);

				Internal::GetDataTypeManager().CopyData(copiedInputPinType.GetDataTypeID(), createdInputPin.GetDataPtr(), copiedInputPin.GetDataPtr());
			}
		}
	}

	void PasteCopyBuffer(NodeGraphProxy aTargetNodeGraphProxy, const Vec2 aPosition, CommandTracker* const aCommandTracker)
	{
		const NodeGraph& nodeGraphCopy = Internal::GetNodeGraphCopy().GetNodeGraph();

		if (aCommandTracker)
		{
			aCommandTracker->BeginComposite("Paste Nodes");
		}

		NodeGraph& targetNodeGraph = aTargetNodeGraphProxy.GetNodeGraph();

		std::unordered_map<NodeID, NodeID> nodeConverter;

		for (NodeID sourceNodeID{ 0 }; sourceNodeID < nodeGraphCopy.GetNodeCount(); sourceNodeID++)
		{
			const Node& node = nodeGraphCopy.GetNode(sourceNodeID);
			const NodeID createdNodeID = Internal::CreateNode(aTargetNodeGraphProxy.GetVariant(), node.GetTypeID(), aPosition + node.GetPosition(), aCommandTracker);
			nodeConverter.emplace(sourceNodeID, createdNodeID);

			const Node& createdNode = Internal::GetNode(createdNodeID, targetNodeGraph);
			DataTypeManager& dataTypeManager = Internal::GetDataTypeManager();
			for (const PinID createdInputPinID : createdNode.GetInputPins())
			{
				Pin& createdInputPin = Internal::GetPin(createdInputPinID, targetNodeGraph);
				const PinType& createdInputPinType = Internal::GetPinTypeManager().GetPinType(createdInputPin.GetTypeID());

				const PinID sourcePinID = Internal::GetOpposingPinID(targetNodeGraph, createdInputPinID, nodeGraphCopy, sourceNodeID);
				const Pin& sourcePin = nodeGraphCopy.GetPin(sourcePinID);
				dataTypeManager.CopyData(createdInputPinType.GetDataTypeID(), createdInputPin.GetDataPtr(), sourcePin.GetDataPtr());
			}
		}

		for (const Link& link : nodeGraphCopy.GetLinks())
		{
			const Pin& inputPin = nodeGraphCopy.GetPin(link.GetInputPinID());
			const Pin& outputPin = nodeGraphCopy.GetPin(link.GetOutputPinID());
			const PinID createdInputPinID = Internal::GetOpposingPinID(nodeGraphCopy, link.GetInputPinID(), targetNodeGraph, nodeConverter.at(inputPin.GetNodeID()));
			const PinID createdOutputPinID = Internal::GetOpposingPinID(nodeGraphCopy, link.GetOutputPinID(), targetNodeGraph, nodeConverter.at(outputPin.GetNodeID()));
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
		const auto& functions = Internal::GetNodeTypeManager().GetFunctions();
		std::vector<FunctionProxy> views;
		views.reserve(functions.size());

		for (FunctionID functionID{ 0 }; functionID < functions.size(); ++functionID)
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

	std::vector<NodeTypeProxy> GetNodeTypesFilteredByDataTypeAndFlowType(const GenericDataTypeID aDataTypeID, const eIODirection aIODirection)
	{
		return GetNodeTypesFiltered([aDataTypeID, aIODirection](const NodeType& aNodeType) -> bool
			{
				const std::vector<PinTypeID>& pinTypeIDs = SelectByIODirection(aIODirection, aNodeType.GetInputPinTypeIDs(), aNodeType.GetOutputPinTypeIDs());
				for (const PinTypeID pinTypeID : pinTypeIDs)
				{
					const PinType& pinType = Internal::GetPinTypeManager().GetPinType(pinTypeID);
					if (pinType.GetDataTypeID() == aDataTypeID)
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

	std::vector<NodeTypeProxy> GetNodeTypesFilteredByRelatedDataTypesAndFlowTypeAndTrait(const GenericDataTypeID aDataTypeID, const eIODirection aIODirection, const eNodeTrait aNodeTrait, bool(*aBitOperation)(eNodeTrait, eNodeTrait))
	{
		return GetNodeTypesFiltered([aDataTypeID, aIODirection, aNodeTrait, aBitOperation](const NodeType& aNodeType) -> bool
			{
				if (!aBitOperation(aNodeTrait, aNodeType.GetTraits()))
				{
					return false;
				}
				const std::vector<PinTypeID>& pinTypeIDs = SelectByIODirection(aIODirection, aNodeType.GetInputPinTypeIDs(), aNodeType.GetOutputPinTypeIDs());
				for (const PinTypeID pinTypeID : pinTypeIDs)
				{
					const PinType& pinType = Internal::GetPinTypeManager().GetPinType(pinTypeID);
					const GenericDataTypeID inputDataTypeID = SelectByIODirection(aIODirection, pinType.GetDataTypeID(), aDataTypeID);
					const GenericDataTypeID outputDataTypeID = SelectByIODirection(aIODirection, aDataTypeID, pinType.GetDataTypeID());
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
				return aBitOperation(aNodeTrait, aNodeType.GetTraits());
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

